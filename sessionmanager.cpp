/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief SessionManager class implementation
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#include "sessionmanager.h"
#include "outputmanager.h"
#include "xmodemtransfer.h"

#include <QSerialPortInfo>
#include <QProgressDialog>
#include <QMessageBox>
#include <QThread>
#include <QFile>
#include <QTimer>

SessionManager::SessionManager(QObject *parent) :
    QObject(parent)
{
    serial = new QSerialPort();
    in_progress = false;

    connect(serial, &QSerialPort::readyRead, this, &SessionManager::readData);
    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>
                (&QSerialPort::error), this, &SessionManager::handleError);
}

SessionManager::~SessionManager()
{
    if (serial)
    {
        // closes connection if needed
        if (serial->isOpen())
            serial->close();
        delete serial;
    }
}

void SessionManager::handleError(QSerialPort::SerialPortError serialPortError)
{
    switch (serialPortError)
    {
        // no error
        case QSerialPort::NoError:
            break;

        // recoverable errors : inform user and clear error
        case QSerialPort::OpenError:

            QMessageBox::warning(NULL, tr("Error"), serial->errorString());
            // reset error
            serial->clearError();
            break;

        // unrecoverable errors : inform user and close the port/connection
        default:
            if (in_progress)
            {
                QMessageBox::critical(NULL, tr("Error"), serial->errorString());

                // on some error (ex: hot unplugging) the 'QSerialPort::error' property successively
                // takes multiple values.
                // to prevent from displaying successive error messages, the in_progress flag is
                // set to indicate that we are not interested by next messages, until the user tries to open
                // again the serial port
                in_progress = false;
                if (serial->isOpen())
                {
                    serial->clearError();

                    closeSession();
                }
            }
            break;
    }
}

void SessionManager::openSession(const QHash<QString, QString>& port_cfg)
{
    bool cfg_ok = true, ok;

    // try converting port config from the hash
    QSerialPort::BaudRate baud_rate = static_cast<QSerialPort::BaudRate>
            (port_cfg["baud_rate"].toInt(&ok));
    cfg_ok &= ok;

    QSerialPort::DataBits data_bits = static_cast<QSerialPort::DataBits>
            (port_cfg["data_bits"].toInt(&ok));
    cfg_ok &= ok;

    QSerialPort::Parity parity = static_cast<QSerialPort::Parity>
            (port_cfg["parity"].toInt(&ok));
    cfg_ok &= ok;

    QSerialPort::StopBits stop_bits = static_cast<QSerialPort::StopBits>
            (port_cfg["stop_bits"].toInt(&ok));
    cfg_ok &= ok;

    QSerialPort::FlowControl flow_control = static_cast<QSerialPort::FlowControl>
            (port_cfg["flow_control"].toInt(&ok));
    cfg_ok &= ok;

    // a conversion didn't make it
    Q_ASSERT_X(cfg_ok, "SessionManager::openSession", "a conversion didn't make it");

    // configure port
#if (QT_VERSION < QT_VERSION_CHECK(5, 5, 0)) && defined(Q_OS_MAC)
    // connection error on MacOsX if port name is set with setPortName instead
    // of setPort (issue #7)
    // on OSX, versions prior to Qt5.5 do not prepend device path to device name
    serial->setPort(QSerialPortInfo(port_cfg[QStringLiteral("device")]));
#else
    // tested on linux and windows
    // and this is necessary to make QSerialPort work with pseudo
    // terminal created with socat for example
    serial->setPortName(port_cfg[QStringLiteral("device")]);
#endif
    serial->setBaudRate(baud_rate);
    serial->setDataBits(data_bits);
    serial->setParity(parity);
    serial->setStopBits(stop_bits);
    serial->setFlowControl(flow_control);

    // flag indicating that a connection is in progress (eventually successful or not)
    in_progress = true;

    // open serial port
    if (serial->open(QIODevice::ReadWrite))
    {
        curr_cfg = port_cfg;
        emit sessionOpened();
    }
    else
    {
        // here, stopped means 'no connection is in progress'
        emit sessionClosed();
    }
}

void SessionManager::closeSession()
{
    if (serial->isOpen())
    {
        serial->close();
        emit sessionClosed();
    }
}

bool SessionManager::isSessionOpen() const
{
    return serial->isOpen();
}

void SessionManager::saveToFile(const QByteArray &data)
{
    QFile dump(curr_cfg["dump_file"]);

    // mode is OR'ed with 'Text' flag in "Ascii" mode
    QIODevice::OpenMode mode = QIODevice::Append;

    if (curr_cfg["dump_file"] == QString::number(ConnectDialog::Ascii))
        mode |= QIODevice::Text;

    if (dump.open(mode))
        dump.write(data);
}

void SessionManager::readData()
{
    QByteArray data(serial->readAll());

    emit dataReceived(data);

    // append to dump file if configured
    if (curr_cfg["dump_enabled"] == "1")
        saveToFile(data);
}

void SessionManager::sendToSerial(const QByteArray &data)
{
    serial->write(data);
}

void SessionManager::initFileTransfer(const QString &filename, FileTransferMode type)
{
    QThread *thread = new QThread;
    FileTransfer *file_transfer = 0;

    switch (type)
    {
        case XMODEM:
            file_transfer = new XModemTransfer(0, serial, filename);
        break;
        case YMODEM:
        case ZMODEM:
            Q_ASSERT_X(false, "SessionManager::initFileTransfer", "protocol not yet implemented");
        break;
    }

    // create a timer to update progress
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, file_transfer, &FileTransfer::updateProgress, Qt::DirectConnection);
    timer->start(250);

    QProgressDialog *progress_dlg = new QProgressDialog();
    connect(progress_dlg, &QProgressDialog::canceled, file_transfer, &FileTransfer::cancelTransfer);
    progress_dlg->setRange(0, 100);
    progress_dlg->setWindowModality(Qt::ApplicationModal);

    // move file_transfer worker and serialport instance to newly created thread
    file_transfer->moveToThread(thread);
    serial->moveToThread(thread);

    connect(file_transfer, &FileTransfer::transferEnded, this, &SessionManager::handleTransferEnded);
    connect(thread, &QThread::started, file_transfer, &FileTransfer::startTransfer);
    connect(file_transfer, &FileTransfer::transferProgressed, progress_dlg, &QProgressDialog::setValue);
    connect(file_transfer, &FileTransfer::transferEnded, thread, &QThread::quit);
    connect(file_transfer, &FileTransfer::transferEnded, file_transfer, &FileTransfer::deleteLater);
    connect(file_transfer, &FileTransfer::transferEnded, progress_dlg, &QProgressDialog::close);
    connect(file_transfer, &FileTransfer::transferEnded, progress_dlg, &QProgressDialog::deleteLater);
    connect(file_transfer, &FileTransfer::transferEnded, timer, &QTimer::stop);
    connect(file_transfer, &FileTransfer::transferEnded, timer, &QTimer::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
    progress_dlg->exec();
}

void SessionManager::handleTransferEnded(FileTransfer::TransferError error)
{
    switch (error)
    {
        case FileTransfer::LocalCancelledError:
            return;
        case FileTransfer::NoError:
            QMessageBox::information(NULL, tr("Cutecom-ng"), QStringLiteral("File transferred succesfully"));
            break;
        default:
            QMessageBox::warning(NULL, tr("Cutecom-ng"), FileTransfer::errorString(error));
            break;
    }
}
