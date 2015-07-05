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

#include <algorithm>
#include <iterator>

#include <QUiLoader>
#include <QLineEdit>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"
#include "sessionmanager.h"
#include "outputmanager.h"

/// line ending char appended to the commands sent to the serial port
const QString LINE_ENDING = "\n";

/// maximum count of document blocks for the bootom output
const int MAX_OUTPUT_LINES = 100;

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    search_widget(0),
    search_input(0)
{
    ui->setupUi(this);

    // create session and output managers
    output_mgr = new OutputManager(this);
    session_mgr = new SessionManager(this);
    connect_dlg = new ConnectDialog(this);

    // load search widget and hide it
    QUiLoader loader;
    QFile file(":/searchwidget.ui");
    file.open(QFile::ReadOnly);
    search_widget = loader.load(&file, ui->mainOutput);
    Q_ASSERT_X(search_widget, "MainWindow::MainWindow", "error while loading searchwidget.ui");
    file.close();

    search_input = search_widget->findChild<QLineEdit*>("searchInput");

    Q_ASSERT_X(search_input, "MainWindow::MainWindow", "didn't find searchInput");

    // to make widget appear on top of : NOT WORKING
    search_widget->hide();

    // show connection dialog
    connect(ui->connectButton, &QAbstractButton::clicked, connect_dlg, &ConnectDialog::show);

    // handle reception of new data from serial port
    connect(session_mgr, &SessionManager::dataReceived, this, &MainWindow::handleDataReceived);

    // get data formatted for display and show it in output view
    connect(output_mgr, &OutputManager::dataConverted, this, &MainWindow::addDataToView);

    // get data formatted for display and show it in output view
    connect(ui->inputBox, &HistoryComboBox::lineEntered, this, &MainWindow::handleNewInput);

    // clear output manager buffer at session start
    connect(session_mgr, &SessionManager::sessionStarted, output_mgr, &OutputManager::clear);

    // clear output text
    connect(session_mgr, &SessionManager::sessionStarted, ui->mainOutput, &QPlainTextEdit::clear);

    // call openSession when user accepts/closes connection dialog
    connect(connect_dlg, &ConnectDialog::openDeviceClicked, session_mgr, &SessionManager::openSession);

    connect(ui->splitOutputBtn, &QToolButton::clicked, this, &MainWindow::toggleOutputSplitter);

    // additional configuration for bottom output
    ui->bottomOutput->hide();
    ui->bottomOutput->document()->setMaximumBlockCount(MAX_OUTPUT_LINES);
    ui->bottomOutput->viewport()->installEventFilter(this);

    connect(ui->searchButton, &QToolButton::toggled, search_widget, &QWidget::setVisible);
}


MainWindow::~MainWindow()
{
    delete ui;
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
    // problem : QTextEdit interprets a '\r' as a new line, so if a buffer ending
    //           with '\r\n' happens to be cut in the middle, there will be 1 extra
    //           line jump in the QTextEdit. To prevent we remove ending '\r' and
    //           prepend them to the next received buffer

    // flag indicating that the previously received buffer ended with CR
    static bool prev_ends_with_CR = false;

    QString newdata;
    if (prev_ends_with_CR)
    {
        // CR was removed at the previous buffer, so now we prepend it
        newdata.append('\r');
        prev_ends_with_CR = false;
    }

    if (textdata.length() > 0)
    {
        QString::const_iterator end_cit = textdata.cend();
        if (textdata.endsWith('\r'))
        {
            // if buffer ends with CR, we don't copy it
            end_cit--;
            prev_ends_with_CR = true;
        }
        std::copy(textdata.begin(), end_cit, std::back_inserter(newdata));
    }

    // record end cursor position before adding text
    QTextCursor prev_end_cursor(ui->mainOutput->document());
    prev_end_cursor.movePosition(QTextCursor::End);

    if (ui->bottomOutput->isVisible())
    {
        // append text to the top output and stay at current position
        QTextCursor cursor(ui->mainOutput->document());
        cursor.movePosition(QTextCursor::End);
        cursor.insertText(newdata);
    }
    else
    {
        // append text to the top output and scroll down
        ui->mainOutput->moveCursor(QTextCursor::End);
        ui->mainOutput->insertPlainText(newdata);
    }

    // append text to bottom output and scroll
    ui->bottomOutput->moveCursor(QTextCursor::End);
    ui->bottomOutput->insertPlainText(newdata);


//    handleSearchTextChanged(search_input->text());
}

void MainWindow::handleDataReceived(const QByteArray &data)
{
    (*output_mgr) << data;
}

void MainWindow::toggleOutputSplitter()
{
    ui->bottomOutput->setVisible(!ui->bottomOutput->isVisible());
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::Wheel)
        return true;

    // base class behaviour
    return QMainWindow::eventFilter(target, event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    // todo :remove hardcoded values
    search_widget->setGeometry(event->size().width() - search_widget->width() - 40, -2, 300, 40);

    // base class implementations
    QMainWindow::resizeEvent(event);
}
