/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief manage serial port session
 *
 * \version $Id: $
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QObject>

#include "connectdialog.h"

class QSerialPort;

class SessionManager : public QObject
{
    Q_OBJECT

private:
    QSerialPort * serial;

public:

    explicit SessionManager(QObject *parent = 0);
    ~SessionManager();

signals:

public slots:

    /**
     * @brief open a serial port connection
     * @param port_cfg serial port settings
     */
    void openSession(const serial_port_cfg_t& port_cfg);
};

#endif // SESSIONMANAGER_H
