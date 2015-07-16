/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief XModemTransfer class implementation
 *
 * \author Aurelien Rainone <aurelien@develer.com>
 */

#include <QtSerialPort>

#include "xmodemtransfer.h"
#include "xmodem.h"

/**
 * \brief g_serial global variable representing the serial port currently
 *                 opened this is needed for _inbyte and _outbyte, functions
 *                 required by the xmodem library
 */
QSerialPort *g_serial = 0;

/**
 * \brief _quit global variable used to indicate to the xmodem library that
 *              current transfer has to be stopped prematurily
 */
bool _quit = false;

/**
 * \brief _progress global variable representing total bytes sent
 */
qint64 _byte_sent = 0;

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
 * \brief _outbyte write 1 byte to the serial port
 * \param c byte to write
 */
void _outbyte(int c)
{
    QByteArray data(1, (char)c);
    if (g_serial->isOpen())
    {
        g_serial->write(data);
        ++_byte_sent;
    }
}


XModemTransfer::XModemTransfer(QObject *parent, QSerialPort *serial, const QString &filename)
    : FileTransfer(parent, serial, filename)
{
    _quit = false;
    _byte_sent = 0;
}

void XModemTransfer::performTransfer()
{
    // set global QSerialPort pointer used by _inbyte/_outbyte
    g_serial = serial;

    TransferError ret;

    // call xmodem transmission routine
    int errcode = xmodemTransmit((unsigned char*)buffer.data(), buffer.size());

    switch (errcode)
    {
        case -6:
            ret = LocalCancelledError;
            break;
        case -5:
            ret = UnknownError;
            break;
        case -4:
            ret = TransmissionError;
            break;
        case -2:
            ret = NoSyncError;
            break;
        case -1:
            ret = RemoteCancelledError;
            break;
        default:
        {
            if (errcode >= 0)
                ret = NoError;      // success
            else
                ret = UnknownError; // shouldn't be here
            break;
        }
    }

    emit transferEnded(ret);
}

void XModemTransfer::cancelTransfer()
{
    _quit = true;
}

void XModemTransfer::updateProgress()
{
    setSentBytes(_byte_sent);
}
