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

public:

    explicit SessionManager(QObject *parent = 0);
    ~SessionManager();

    /**
     * \brief open a serial port connection
     * \param port_cfg    serial port settings
     */
    void openSession(const QHash<QString, QString> &port_cfg);

    /**
     * \brief send data to serial port
     * \param data    byte array data
     */
    void sendToSerial(const QByteArray &data);

signals:

    /**
     * \brief signal emitted when new data has been received from the serial port
     * \param data    byte array data
     */
    void dataReceived(const QByteArray &data);

private:

    /**
     * \brief read data from serial port
     */
    void readData();
};

#endif // SESSIONMANAGER_H
