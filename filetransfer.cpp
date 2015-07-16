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
#include <QTimer>
#include <QDebug>

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
    qDebug() << "FileTransfer::~FileTransfer(), progress_timer: " << progress_timer;

    if (progress_timer)
        delete progress_timer;
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

        thread = new QThread;
        moveToThread(thread);

        progress_timer = new QTimer;

        // and move both serialport and filetransfer (this) instance
        serial->moveToThread(thread);

        // call child class performTransfer() when thread starts
        connect(thread, &QThread::started, this, &FileTransfer::performTransfer);

        connect(progress_timer, &QTimer::timeout, this,
                &FileTransfer::updateProgress);
        progress_timer->start(250);

        connect(this, &FileTransfer::transferFinished,
                progress_timer, &QTimer::stop);

        connect(this, &FileTransfer::transferEnded,
                this, &FileTransfer::handleTransferEnded);

        connect(this, &FileTransfer::transferFinished, this, &FileTransfer::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        thread->start();
        return true;
    }
    else
    {
        emit transferEnded(InputFileError);
        emit transferFinished();
    }

    return false;
}

void FileTransfer::handleTransferEnded(TransferError error)
{
    // be sure that
    if (error == NoError)
        setSentBytes(total_size);

    // move serial instance back to main thread
    serial->moveToThread(QApplication::instance()->thread());
    thread->quit();
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
