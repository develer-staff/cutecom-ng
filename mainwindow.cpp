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
#include "connectdialog.h"
#include "ui_mainwindow.h"
#include "sessionmanager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->connectButton, &QAbstractButton::clicked, this, &MainWindow::openConnectionDialog);

    session_mgr = new SessionManager(this);
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

