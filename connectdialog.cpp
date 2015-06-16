/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Serial port connection and settings dialog
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#include "connectdialog.h"
#include "ui_connectdialog.h"

#include <QList>
#include <QHash>
#include <QtSerialPort>
#include <QSerialPortInfo>

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);

    // fill the combo box values
    fillSettingsLists();

    // define the default values
    defaultValues["device"] = "";
    defaultValues["baud_rate"] = QString::number(QSerialPort::Baud115200);
    defaultValues["data_bits"] = QString::number(QSerialPort::Data8);
    defaultValues["stop_bits"] = QString("1");
    defaultValues["parity"] = QString("None");
    defaultValues["flow_control"] = QString("None");

    preselectPortSettings(defaultValues);
}

ConnectDialog::~ConnectDialog()
{
    delete ui;
}


void ConnectDialog::fillSettingsLists()
{
    // fill devices combo box
    foreach(QSerialPortInfo info, QSerialPortInfo::availablePorts())
        ui->deviceList->addItem(info.portName(), info.portName());

    // fill baud rates combo box
    QStringList baud_rates;
    baud_rates <<
        QString::number(QSerialPort::Baud1200) << QString::number(QSerialPort::Baud2400);
    baud_rates <<
        QString::number(QSerialPort::Baud4800) << QString::number(QSerialPort::Baud9600);
    baud_rates <<
        QString::number(QSerialPort::Baud19200) << QString::number(QSerialPort::Baud38400);
    baud_rates <<
        QString::number(QSerialPort::Baud57600) << QString::number(QSerialPort::Baud115200); 
    ui->baudRateList->addItems(baud_rates);

    // fill data bits combo box
    QStringList data_bits;
    data_bits <<
        QString::number(QSerialPort::Data5) << QString::number(QSerialPort::Data6);
    data_bits <<
        QString::number(QSerialPort::Data7) << QString::number(QSerialPort::Data8);
    ui->dataBitsList->addItems(baud_rates);

    // fill stop bits combo box
    ui->stopBitsList->addItem("1", QSerialPort::OneStop);
#ifdef Q_WS_WIN
    ui->stopBitsList->addItem("1.5", QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitsList->addItem("2", QSerialPort::TwoStop);

    // fill parity combo box
    ui->parityList->addItem("None", QSerialPort::NoParity);
    ui->parityList->addItem("Even", QSerialPort::EvenParity);
    ui->parityList->addItem("Odd", QSerialPort::OddParity);
    ui->parityList->addItem("Space", QSerialPort::SpaceParity);
    ui->parityList->addItem("Mark", QSerialPort::MarkParity);

    // fill flow control
    ui->flowControlList->addItem("None", QSerialPort::NoFlowControl);
    ui->flowControlList->addItem("Hardware", QSerialPort::HardwareControl);
    ui->flowControlList->addItem("Software", QSerialPort::SoftwareControl);
}

void ConnectDialog::preselectPortSettings(const QHash<QString, QString>& settings)
{
    ui->deviceList->setCurrentText(settings["device"]);
    ui->baudRateList->setCurrentText(settings["baud_rate"]);
    ui->dataBitsList->setCurrentText(settings["data_bits"]);
    ui->stopBitsList->setCurrentText(settings["stop_bits"]);
    ui->parityList->setCurrentText(settings["parity"]);
    ui->flowControlList->setCurrentText(settings["flow_control"]);
}

void ConnectDialog::accept()
{
    // create a serial port config object from current selection
    QHash<QString, QString> cfg;
    cfg["device"] = ui->deviceList->currentText();
    cfg["baud_rate"] = ui->baudRateList->currentText();
    cfg["data_bits"] = ui->dataBitsList->currentText();
    cfg["stop_bits"] = ui->stopBitsList->itemData(
                ui->stopBitsList->currentIndex()).toString();
    cfg["parity"] = ui->parityList->itemData(
                ui->parityList->currentIndex()).toString();
    cfg["flow_control"] = ui->flowControlList->itemData(
                ui->flowControlList->currentIndex()).toString();

    close();

    emit openDeviceClicked(cfg);
}
