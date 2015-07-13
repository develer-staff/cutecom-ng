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

TransferThread::TransferThread(QObject *parent, QSerialPort *serial, const QString &filename) :
    QThread(parent),
    filename(filename),
    serial(serial)
{
    quit = false;
    wait_timeout = -1;
}

TransferThread::~TransferThread()
{
}

void TransferThread::run()
{
    // call child class file transfer protocol implementation
    TransferError ret = this->startTransfer();
    emit transferEnded(ret);
}

void TransferThread::cancelTransfer()
{
    quit = true;
}
