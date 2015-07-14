/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief TransferThread class implementation
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#include "filetransfer.h"
#include <QtSerialPort>
#include <QFile>

FileTransfer::FileTransfer(QObject *parent, QSerialPort *serial, const QString &filename) :
    QObject(parent),
    filename(filename),
    serial(serial)  
{
    total_size = 0;
    cur_progress = 0;
    qRegisterMetaType<TransferError>("TransferError");
}

FileTransfer::~FileTransfer()
{
}

void FileTransfer::startTransfer()
{
    // fill buffer with file content
    buffer.clear();

    {
        QFile file(filename);
        if (file.open(QIODevice::ReadOnly))
        {
            buffer = file.readAll();
            total_size = file.size();
        }
    }

    // call child class file transfer protocol implementation
    TransferError ret = this->performTransfer();
    if (ret != NoError)
        emit transferError(ret);
     emit transferFinished();
}


/**
 * \brief called from child class to indicate that current file transfer
 *        has already sent 'bytes_sent' total bytes
 *        transferProgressed signal will be emitted if necessary
 * \param bytes_sent total amount of bytes sent since transfer start
 */
void FileTransfer::updateTransfered(int bytes_sent)
{
    int percent = 100 * bytes_sent / total_size;
    if (percent > cur_progress)
    {
        // emit transferProgressed if we progressed of at least 1%
        cur_progress = percent;
        transferProgressed(cur_progress);
    }
}
