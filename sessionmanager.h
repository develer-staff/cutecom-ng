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

#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QObject>

#include "connectdialog.h"

class QSerialPort;
class OutputManager;

class SessionManager : public QObject
{
    Q_OBJECT

private:
    QSerialPort * serial;
    OutputManager *output_mgr;

public:

    explicit SessionManager(OutputManager *output_mgr, QObject *parent = 0);
    ~SessionManager();

signals:

public slots:

    /**
     * @brief open a serial port connection
     * @param port_cfg serial port settings
     */
    void openSession(const QHash<QString, QString>& port_cfg);

    /**
     * @brief read data when port is ready
     */
    void readData();
};

#endif // SESSIONMANAGER_H
