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

/**
 * \brief g_serial global variable representing the serial port currently
 *                 opened this is needed for _inbyte and _outbyte, functions
 *                 required by the xmodem library
 */
QSerialPort *g_serial = 0;

/**
 * \brief _inbyte consume 1 byte from serial port
 * \param timeout timeout in miliseconds
 * \return consumed byte or -1 in case of error
 */
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

/**
 * \brief _quit this global variable used to indicate to the xmodem library that
 *              current transfer has to be stopped prematurily
 */
bool _quit = false;

/**
 * \brief _outbyte write 1 byte to the serial port
 * \param c byte to write
 */
void _outbyte(int c)
{
    QByteArray data(1, (char)c);
    if (g_serial->isOpen())
        g_serial->write(data);
}


XModemTransfer::XModemTransfer(QObject *parent, QSerialPort *serial, const QString &filename)
    : FileTransfer(parent, serial, filename)
{
    _quit = false;
}

XModemTransfer::~XModemTransfer()
{
    // TODO???
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

void XModemTransfer::cancelTransfer()
{
    _quit = true;
}
