/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief main cutecom-ng window
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"
#include "sessionmanager.h"
#include "outputmanager.h"
#include "inputline.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->connectButton, &QAbstractButton::clicked, this, &MainWindow::openConnectionDialog);

    // create session and output manager
    OutputManager *output_mgr = new OutputManager(ui->textOutput);
    session_mgr = new SessionManager(output_mgr, this);

    connect(ui->textInput, &InputLine::commandSent, session_mgr, &SessionManager::sendToSerial);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openConnectionDialog()
{
    ConnectDialog dialog(this);

    connect(&dialog, &ConnectDialog::openDeviceClicked, session_mgr, &SessionManager::openSession);
    dialog.exec();
}
