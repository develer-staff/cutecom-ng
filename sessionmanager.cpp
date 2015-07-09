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

#include <QMessageBox>
#include <QSerialPortInfo>
#include <QFile>

SessionManager::SessionManager(QObject *parent)
    : QObject(parent)
{
    serial = new QSerialPort(this);
    in_progress = false;

    connect(serial, &QSerialPort::readyRead, this, &SessionManager::readData);
    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>
                (&QSerialPort::error), this, &SessionManager::handleError);
}

SessionManager::~SessionManager()
{
    // closes connection if needed
    if (serial->isOpen())
        serial->close();
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
                    serial->close();
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

    // closes connection if needed
    if (serial->isOpen())
        serial->close();

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
        emit sessionStarted();
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

