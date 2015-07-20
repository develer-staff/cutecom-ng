/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief FileTransfer class implementation
 *
 * \author Aurelien Rainone <aurelien@develer.com>
 */

#include "filetransfer.h"
#include <QApplication>
#include <QtSerialPort>
#include <QFile>

FileTransfer::FileTransfer(QObject *parent, QSerialPort *serial, const QString &filename) :
    QObject(parent),
    filename(filename),
    serial(serial),
    thread(0)
{
    total_size = 0;
    qRegisterMetaType<TransferError>("TransferError");
}

bool FileTransfer::startTransfer()
{   
    // fill buffer with file content
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly))
    {
        buffer = file.readAll();
        total_size = file.size();
        file.close();
        if (total_size > 0)
        {
            thread = new QThread;

            // and move both serialport and filetransfer (this) instance
            moveToThread(thread);
            serial->moveToThread(thread);

            // call child class performTransfer() when thread starts
            connect(thread, &QThread::started, this, &FileTransfer::performTransfer);

            connect(this, &FileTransfer::transferEnded,
                    this, &FileTransfer::handleTransferEnded);

            connect(thread, &QThread::finished, thread, &QThread::deleteLater);

            thread->start();
            return true;
        }
    }

    handleTransferEnded(InputFileError);
    return false;
}

void FileTransfer::handleTransferEnded(TransferError error)
{
    Q_UNUSED(error)

    // move serial instance back to main thread
    serial->moveToThread(QApplication::instance()->thread());
    thread->quit();
    this->deleteLater();
}

QString FileTransfer::errorString(TransferError error)
{
    switch(error)
    {
        case NoError :
            return QStringLiteral("Success");
        case NoSyncError:
            return QStringLiteral("Synchronization error");
        case TransmissionError:
            return QStringLiteral("Transmission error");
        case TimeoutError:
            return QStringLiteral("Transfer timeout");
        case RemoteCancelledError:
            return QStringLiteral("Transfer cancelled by remote");
        case LocalCancelledError:
            // should not be treated as an error
            return QStringLiteral("Transfer cancelled");
        case InputFileError:
            return QStringLiteral("Can't open input file");
        case UnknownError:
        default:
            return QStringLiteral("Unknown Error");
    }
}
