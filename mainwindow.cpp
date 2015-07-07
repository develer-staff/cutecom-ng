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
#include <QShortcut>

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

    // clear both output text at session start
    connect(session_mgr, &SessionManager::sessionStarted, ui->mainOutput, &QPlainTextEdit::clear);
    connect(session_mgr, &SessionManager::sessionStarted, ui->bottomOutput, &QPlainTextEdit::clear);

    // clear both output text when 'clear' is clicked
    connect(ui->clearButton, &QToolButton::clicked, ui->mainOutput, &QPlainTextEdit::clear);
    connect(ui->clearButton, &QToolButton::clicked, ui->bottomOutput, &QPlainTextEdit::clear);

    // call openSession when user accepts/closes connection dialog
    connect(connect_dlg, &ConnectDialog::openDeviceClicked, session_mgr, &SessionManager::openSession);

    connect(ui->splitOutputBtn, &QToolButton::clicked, this, &MainWindow::toggleOutputSplitter);

    // additional configuration for bottom output
    ui->bottomOutput->hide();
    ui->bottomOutput->document()->setMaximumBlockCount(MAX_OUTPUT_LINES);
    ui->bottomOutput->viewport()->installEventFilter(this);

    // load search widget and hide it
    QUiLoader loader;
    QFile file(":/searchwidget.ui");
    file.open(QFile::ReadOnly);
    search_widget = loader.load(&file, ui->mainOutput);
    Q_ASSERT_X(search_widget, "MainWindow::MainWindow", "error while loading searchwidget.ui");

    search_input = search_widget->findChild<QLineEdit*>("searchInput");
    Q_ASSERT_X(search_input, "MainWindow::MainWindow", "didn't find searchInput");

    search_prev_button = search_widget->findChild<QToolButton*>("previousButton");
    search_next_button = search_widget->findChild<QToolButton*>("nextButton");
    Q_ASSERT_X(search_prev_button, "MainWindow::MainWindow", "didn't find previousButton");
    Q_ASSERT_X(search_next_button, "MainWindow::MainWindow", "didn't find nextButton");

    file.close();
    search_widget->hide();

    // create the search results highlighter for main output
    SearchHighlighter *search_highlighter_main = new SearchHighlighter(ui->mainOutput->document());
    connect(search_input, &QLineEdit::textChanged, search_highlighter_main, &SearchHighlighter::setSearchString);

    // search result highlighter (without search cursor) for bottom output
    SearchHighlighter *search_highlighter_bottom = new SearchHighlighter(ui->bottomOutput->document(), false);
    connect(search_input, &QLineEdit::textChanged, search_highlighter_bottom, &SearchHighlighter::setSearchString);

    // connect search-related signals/slots
    connect(search_prev_button, &QToolButton::clicked, search_highlighter_main, &SearchHighlighter::previousOccurence);
    connect(search_next_button, &QToolButton::clicked, search_highlighter_main, &SearchHighlighter::nextOccurence);
    connect(search_highlighter_main, &SearchHighlighter::cursorPosChanged, this, &MainWindow::handleCursosPosChanged);
    connect(search_highlighter_main, &SearchHighlighter::totalOccurencesChanged, this, &MainWindow::handleTotalOccurencesChanged);
    connect(ui->searchButton, &QToolButton::toggled, this, &MainWindow::showSearchWidget);

    search_input->installEventFilter(this);
    ui->mainOutput->viewport()->installEventFilter(this);
    installEventFilter(this);
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
    if (target == ui->mainOutput->viewport())
    {
        if (event->type() == QEvent::Resize)
        {
            // re position search widget when main output inner size changes
            // this takes into account existence of vertical scrollbar
            QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event);
            search_widget->move(resizeEvent->size().width() - search_widget->width(), 0);
        }
        else if (event->type() == QEvent::Wheel)
        {
            // allow mouse wheel usage only in 'browsing mode'
            if (!ui->bottomOutput->isVisible())
                return true;
        }
    }
    else
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (ui->searchButton->isChecked())
            {
                // hide search widget on Escape key press
                if (keyEvent->key() == Qt::Key_Escape)
                    emit ui->searchButton->toggle();
            }
            else
            {
                // show search widget on Ctrl-F
                if (keyEvent->key() == Qt::Key_F && keyEvent->modifiers() == Qt::ControlModifier)
                    emit ui->searchButton->toggle();
            }
            if (target == search_input)
            {
                if (keyEvent->key() == Qt::Key_Return)
                {
                    if (keyEvent->modifiers() == Qt::ShiftModifier)
                        emit search_prev_button->click();
                    else
                        emit search_next_button->click();
                }
            }

        }
    }

    // base class behaviour
    return QMainWindow::eventFilter(target, event);
}

void MainWindow::showSearchWidget(bool show)
{
    // record which widget had focus before showing search widget, in order
    // to return focus to it when search widget is hidden
    static QWidget *prevFocus = 0;

    QPropertyAnimation *animation = new QPropertyAnimation(search_widget, "pos");
    animation->setDuration(150);

    // place search widget at the rightmost position of mainoutput viewport
    QPoint showed_pos(ui->mainOutput->viewport()->width() - search_widget->width(), 0);
    QPoint hidden_pos(ui->mainOutput->viewport()->width() - search_widget->width(), -search_widget->height());
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

void MainWindow::handleTotalOccurencesChanged(int total_occurences)
{
    if (total_occurences == 0)
        search_input->setStyleSheet("QLineEdit{background-color: red;}");
    else
        search_input->setStyleSheet("");
}
