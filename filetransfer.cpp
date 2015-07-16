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
    cur_progress = 0;
    qRegisterMetaType<TransferError>("TransferError");
}

FileTransfer::~FileTransfer()
{
    // move serial instance back to main thread
    serial->moveToThread(QApplication::instance()->thread());
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

        // create a new thread...
        thread = new QThread;
        moveToThread(thread);

        // and move both serialport and filetransfer (this) instance
        serial->moveToThread(thread);

        // call child class performTransfer() when tread starts
        connect(thread, &QThread::started, this, &FileTransfer::performTransfer);

        // stop and clean thread when file transfer has ended
        connect(this, &FileTransfer::transferEnded, thread, &QThread::quit);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        thread->start();
        return true;
    }
    else
    {
        emit transferEnded(InputFileError);
    }

    return false;
}

void FileTransfer::setSentBytes(int bytes_sent)
{
    if (total_size)
    {
        int percent = 100 * bytes_sent / total_size;
        if (percent > cur_progress)
        {
            // emit transferProgressed if we progressed of at least 1%
            cur_progress = percent;
            emit transferProgressed(cur_progress);
        }
    }
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
