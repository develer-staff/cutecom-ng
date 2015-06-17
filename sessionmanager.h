/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief SessionManager class header
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QObject>

#include "connectdialog.h"

class QSerialPort;
class OutputManager;


/**
 * \brief manage serial port session
 */
class SessionManager : public QObject
{
    Q_OBJECT

private:
    QSerialPort   *serial;
    OutputManager *output_mgr;

public:

    explicit SessionManager(OutputManager *output_mgr, QObject *parent = 0);
    ~SessionManager();

    /**
     * \brief open a serial port connection
     * \param port_cfg    serial port settings
     */
    void openSession(const QHash<QString, QString>& port_cfg);

    /**
     * \brief read data from serial port
     */
    void readData();

    /**
     * \brief send data to serial port
     * \param data    byte array data
     */
    void sendToSerial(const QByteArray &data);

};

#endif // SESSIONMANAGER_H
