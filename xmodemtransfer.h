/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief XModemTransfer class header
 *
 * \author Aurelien Rainone <aurelien@develer.com>
 */

#ifndef XMODEMTRANSFER_H
#define XMODEMTRANSFER_H

#include "filetransfer.h"

/**
 * \brief xmodem protocol implementation of FileTransfer
 */
class XModemTransfer : public FileTransfer
{
    Q_OBJECT

public:

    /**
     * \brief create a XModem transfer thread
     * \param parent   object taking ownership
     * \param serial   opened instance of QSerialPort
     * \param filename file to transfer
     */
    XModemTransfer(QObject *parent, QSerialPort *serial, const QString &filename);

private:

    /**
     * \brief initiate an XMODEM protocol file transfer
     * \return transfer end code
     */
    void performTransfer();
};

#endif // XMODEMTRANSFER_H
