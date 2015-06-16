/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief manage serial port session
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#include "sessionmanager.h"

#include <QSerialPort>
#include <QDebug>

SessionManager::SessionManager(QObject *parent) : QObject(parent)
{
    serial = new QSerialPort(this);
}

SessionManager::~SessionManager()
{

}

void SessionManager::openSession(const serial_port_cfg_t& port_cfg)
{
    Q_UNUSED(port_cfg)

    qDebug() << "SessionManager::openSession(port_cfg)";
    qDebug() << "\twith port_cfg:";
    qDebug() << port_cfg;
}
