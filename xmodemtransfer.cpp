/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief XModemTransfer class implementation
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#include <QtSerialPort>
#include <QMutex>
#include <QTime>

#include "xmodemtransfer.h"
#include "xmodem.h"

QSerialPort *g_serial = 0;

int _inbyte(unsigned short timeout)
{
    char c = 0;

    if (g_serial->isOpen())
    {
        // blocking
        if (g_serial->waitForReadyRead(timeout))
            if (g_serial->getChar(&c))
                return c;
    }
    return -1;
}

void _outbyte(int c)
{
    QByteArray data(1, c);
    if (g_serial->isOpen())
        g_serial->write(data);
}


XModemTransfer::XModemTransfer(QObject *parent, QSerialPort *serial, const QString &filename)
    : FileTransfer(parent, serial, filename)
{
}

XModemTransfer::~XModemTransfer()
{
    should_quit = true;
    wait();
}

FileTransfer::TransferError XModemTransfer::performTransfer()
{
    // set global serialport pointer for xmodem library
    g_serial = serial;

    switch (xmodemTransmit((unsigned char*)buffer.data(), buffer.size()))
    {
        case -5:
            return UnknownError;
            break;
        case -4:
            return TransmissionError;
            break;
        case -2:
            return NoSyncError;
            break;
        case -1:
            return RemoteCancelledError;
            break;
    }

    return NoError;
}
