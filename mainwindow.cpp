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
#include <QPropertyAnimation>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"
#include "sessionmanager.h"
#include "outputmanager.h"
#include "searchhighlighter.h"

/// line ending char appended to the commands sent to the serial port
const QString LINE_ENDING = "\n";

/// maximum count of document blocks for the bootom output
const int MAX_OUTPUT_LINES = 100;

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

    search_input = search_widget->findChild<QLineEdit*>("searchInput");
    Q_ASSERT_X(search_input, "MainWindow::MainWindow", "didn't find searchInput");

    QToolButton * searchPrevButton = search_widget->findChild<QToolButton*>("previousButton");
    QToolButton * searchNextButton = search_widget->findChild<QToolButton*>("nextButton");
    Q_ASSERT_X(searchPrevButton, "MainWindow::MainWindow", "didn't find previousButton");
    Q_ASSERT_X(searchNextButton, "MainWindow::MainWindow", "didn't find nextButton");

    file.close();
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
    connect(session_mgr, &SessionManager::sessionStarted, ui->bottomOutput, &QPlainTextEdit::clear);

    // call openSession when user accepts/closes connection dialog
    connect(connect_dlg, &ConnectDialog::openDeviceClicked, session_mgr, &SessionManager::openSession);

    connect(ui->splitOutputBtn, &QToolButton::clicked, this, &MainWindow::toggleOutputSplitter);

    // additional configuration for bottom output
    ui->bottomOutput->hide();
    ui->bottomOutput->document()->setMaximumBlockCount(MAX_OUTPUT_LINES);
    ui->bottomOutput->viewport()->installEventFilter(this);

    // create the search results highlighter and connect search-related signals/slots
    search_highlighter = new SearchHighlighter(ui->mainOutput->document());
    connect(ui->searchButton, &QToolButton::toggled, this, &MainWindow::showSearchWidget);
    connect(search_input, &QLineEdit::textChanged, search_highlighter, &SearchHighlighter::setSearchString);
    connect(searchPrevButton, &QToolButton::clicked, search_highlighter, &SearchHighlighter::previousOccurence);
    connect(searchNextButton, &QToolButton::clicked, search_highlighter, &SearchHighlighter::nextOccurence);

    connect(search_highlighter, &SearchHighlighter::cursorPosChanged, this, &MainWindow::handleCursosPosChanged);

    search_input->installEventFilter(this);
    ui->mainOutput->viewport()->installEventFilter(this);
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
    else if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape && search_widget->isVisible())
        {
            // hide search widget on Escape key press
            emit ui->searchButton->toggle();
        }
    }
    else if (event->type() == QEvent::Resize && target == ui->mainOutput->viewport())
    {
        // re position search widget when main output inner size changes
        // this takes into account vertical scrollbar visibility
        QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event);
        search_widget->move(resizeEvent->size().width() - search_widget->width(), 0);
    }

    // base class behaviour
    return QMainWindow::eventFilter(target, event);
}

void MainWindow::showSearchWidget(bool show)
{
    // record which widget had focus before showing search widget, in
    // to return focus to it when search widget is hidden
    static QWidget *prevFocus = 0;

    QPropertyAnimation *animation = new QPropertyAnimation(search_widget, "geometry");
    animation->setDuration(150);

    // arbitrary offset chosen to be way bigger than any scrollbar width, on any platform
    const QRect rect(search_widget->geometry());
    QRect showed_pos(ui->mainOutput->viewport()->width() - rect.width(), 0, rect.width(), rect.height());
    QRect hidden_pos(ui->mainOutput->viewport()->width() - rect.width(), -rect.height(), rect.width(), rect.height());

    animation->setStartValue(show ? hidden_pos : showed_pos);
    animation->setEndValue(show ? showed_pos : hidden_pos);

    if (show)
    {
        prevFocus = QApplication::focusWidget();
        search_widget->setVisible(show);
        search_input->setFocus();
    }
    else
    {
        connect(animation, &QPropertyAnimation::destroyed, search_widget, &QWidget::hide);
        prevFocus->setFocus();
    }
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::handleCursosPosChanged(int pos)
{
    // only in browsing mode
    if (ui->bottomOutput->isVisible())
    {
        // move cursor
        QTextCursor text_cursor = ui->mainOutput->textCursor();
        text_cursor.setPosition(pos);

        // ensure search result cursor is visible
        ui->mainOutput->ensureCursorVisible();
        ui->mainOutput->setTextCursor(text_cursor);
    }
}
