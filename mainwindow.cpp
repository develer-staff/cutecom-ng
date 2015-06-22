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

#include <QScrollBar>

const QString LINE_ENDING = "\n";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->connectButton, &QAbstractButton::clicked, this, &MainWindow::openConnectionDialog);

    // create session and output managers
    output_mgr = new OutputManager(this);
    session_mgr = new SessionManager(this);

    // let output manager handle new data coming from serial port
    connect(session_mgr, &SessionManager::dataReceived, output_mgr, &OutputManager::handleNewData);

    // get data formatted for display and show it in output view
    connect(output_mgr, &OutputManager::dataConverted, this, &MainWindow::addDataToView);

    // get data formatted for display and show it in output view
    connect(ui->inputBox, &HistoryComboBox::lineEntered, this, &MainWindow::handleNewInput);
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

void MainWindow::handleNewInput(QString entry)
{
    // if session is not open, this also keeps user input
    if (session_mgr->isSessionOpen())
    {
        entry.append(LINE_ENDING);
        session_mgr->sendToSerial(entry.toLocal8Bit());
        ui->inputBox->clearEditText();
    }
}

void MainWindow::addDataToView(const QString & textdata)
{
    // save current text selection
    QTextCursor cursor = ui->textOutput->textCursor();

    // insert data at end of 'edit' (but this clears any selection)
    ui->textOutput->moveCursor(QTextCursor::End);
    ui->textOutput->insertPlainText(textdata);

    // revert text selection
    ui->textOutput->setTextCursor(cursor);

    // push scroll to the bottom
    QScrollBar *vbar = ui->textOutput->verticalScrollBar();
    vbar->setValue(vbar->maximum());
}
