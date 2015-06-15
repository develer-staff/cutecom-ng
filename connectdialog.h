/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief Serial port connection and settings dialog
 *
 * \version $Id: $
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectDialog;
}

/**
 * @brief Represent the serial port connection settings
 *
 * Has the following keys :
 *  - "device"
 *  - "baud_rate"
 *  - "data_bits"
 *  - "stop_bits"
 *  - "parity"
 *  - "flow_control"
 * All values are QStrings
 */
typedef QHash<QString, QString> serial_port_cfg_t;


/**
 * @brief The ConnectDialog class
 */
class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectDialog(QWidget *parent = 0);
    ~ConnectDialog();

    void accept();

private:
    Ui::ConnectDialog *ui;
    serial_port_cfg_t defaultValues;

private:
    /**
     * fill connection settings combo boxes
     */
    void fillSettingsLists();

    /**
     * preselect serial port connection settings
     */
    void preselectPortSettings(const serial_port_cfg_t& settings);


signals:

    void openDeviceClicked(const serial_port_cfg_t& config);
};

#endif // CONNECTDIALOG_H
