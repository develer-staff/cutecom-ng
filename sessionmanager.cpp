/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief SessionManager class implementation
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#include "sessionmanager.h"
#include "outputmanager.h"

#include <QMessageBox>
#include <QSerialPort>
#include <QDebug>

SessionManager::SessionManager(QObject *parent)
    : QObject(parent)
{
    serial = new QSerialPort(this);

    connect(serial, &QSerialPort::readyRead, this, &SessionManager::readData);
}

SessionManager::~SessionManager()
{
    // closes connection if needed
    if (serial->isOpen())
        serial->close();
}

void SessionManager::openSession(const QHash<QString, QString>& port_cfg)
{
    Q_UNUSED(port_cfg)

    qDebug() << "SessionManager::openSession(port_cfg)";
    qDebug() << "\twith port_cfg:";
    qDebug() << port_cfg;

    bool cfg_ok = true, ok;

    // try converting port config from the hash
    QSerialPort::BaudRate baud_rate = static_cast<QSerialPort::BaudRate>
            (port_cfg["baud_rate"].toInt(&ok));
    cfg_ok &= ok;

    QSerialPort::DataBits data_bits = static_cast<QSerialPort::DataBits>
            (port_cfg["data_bits"].toInt(&ok));
    cfg_ok &= ok;

    QSerialPort::Parity parity = static_cast<QSerialPort::Parity>
            (port_cfg["parity"].toInt(&ok));
    cfg_ok &= ok;

    QSerialPort::StopBits stop_bits = static_cast<QSerialPort::StopBits>
            (port_cfg["stop_bits"].toInt(&ok));
    cfg_ok &= ok;

    QSerialPort::FlowControl flow_control = static_cast<QSerialPort::FlowControl>
            (port_cfg["flow_control"].toInt(&ok));
    cfg_ok &= ok;

    if (cfg_ok)
    {
        // closes connection if needed
        if (serial->isOpen())
            serial->close();

        // open serial port
        serial->setPortName(port_cfg["device"]);
        serial->setBaudRate(baud_rate);
        serial->setDataBits(data_bits);
        serial->setParity(parity);
        serial->setStopBits(stop_bits);
        serial->setFlowControl(flow_control);

        if (serial->open(QIODevice::ReadWrite))
        {
            qDebug() << "Connected to : " << port_cfg;
        }
        else
        {
            QMessageBox::critical(NULL, tr("Error"), serial->errorString());
        }
    }
    else
    {
        // a conversion didn't make it
        qDebug() << "badly formatted config value :\n" << port_cfg;
    }
}


void SessionManager::readData()
{
    QByteArray data(serial->readAll());

    emit dataReceived(data);
}


void SessionManager::sendToSerial(const QByteArray &data)
{
    serial->write(data);
}
