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

#include "transferthread.h"
#include <QtSerialPort>
#include <QFile>

FileTransfer::FileTransfer(QObject *parent, QSerialPort *serial, const QString &filename) :
    QObject(parent),
    filename(filename),
    serial(serial)
{
    should_quit = false;
    wait_timeout = -1;

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
            buffer = file.readAll();
    }

    // call child class file transfer protocol implementation
    TransferError ret = this->performTransfer();
    if (ret != NoError)
        emit transferError(ret);
     emit transferFinished();
}

void FileTransfer::cancelTransfer()
{
    should_quit = true;
}
