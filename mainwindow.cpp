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

#include <QTextEdit>
#include <QScrollBar>

const QString LINE_ENDING = "\r\n";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->connectButton, &QAbstractButton::clicked, this, &MainWindow::openConnectionDialog);

    // create session and output managers
    output_mgr = new OutputManager(this);
    session_mgr = new SessionManager(this);

    // handle new user input, and send it to the serial port via MainWindow::handleReturnPressed
    connect(ui->textInput, &QLineEdit::returnPressed, this, &MainWindow::handleReturnPressed);

    // let output manager handle new data coming from serial port
    connect(session_mgr, &SessionManager::dataReceived, output_mgr, &OutputManager::handleNewData);

    // get data formatted for display and show it in output view
    connect(output_mgr, &OutputManager::dataConverted, this, &MainWindow::addDataToView);
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

void MainWindow::handleReturnPressed()
{
    QString line = ui->textInput->text() + LINE_ENDING;
    session_mgr->sendToSerial(line.toLocal8Bit());
    ui->textInput->clear();
}

void MainWindow::addDataToView(const QString & textdata)
{
    // save current text selection
    QTextCursor cursor = ui->textOutput->textCursor();

    // insert data at end of 'edit' (but this clears any selection)
    ui->textOutput->moveCursor(QTextCursor::End);
    ui->textOutput->insertPlainText (textdata);

    // revert text selection
    ui->textOutput->setTextCursor(cursor);

    // push scroll to the bottom
    QScrollBar *vbar = ui->textOutput->verticalScrollBar();
    vbar->setValue(vbar->maximum());
}
