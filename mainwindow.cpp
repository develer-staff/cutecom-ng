/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief main cutecom-ng window
 *
 * \author Aurelien Rainone <aurelien@develer.com>
 */

#include <algorithm>
#include <iterator>

#include <QUiLoader>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QShortcut>
#include <QFileDialog>
#include <QProgressDialog>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"
#include "sessionmanager.h"
#include "outputmanager.h"
#include "searchhighlighter.h"

/// maximum count of document blocks for the bootom output
const int MAX_OUTPUT_LINES = 100;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    search_widget(0),
    search_input(0),
    progress_dialog(0)
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

    // handle start/stop session
    connect(session_mgr, &SessionManager::sessionOpened, this, &MainWindow::handleSessionOpened);
    connect(session_mgr, &SessionManager::sessionClosed, this, &MainWindow::handleSessionClosed);

    // clear both output text when 'clear' is clicked
    connect(ui->clearButton, &QToolButton::clicked, ui->mainOutput, &QPlainTextEdit::clear);
    connect(ui->clearButton, &QToolButton::clicked, ui->bottomOutput, &QPlainTextEdit::clear);

    // connect open/close session slots
    connect(connect_dlg, &ConnectDialog::openDeviceClicked, session_mgr, &SessionManager::openSession);
    connect(ui->disconnectButton, &QToolButton::clicked, session_mgr, &SessionManager::closeSession);

    connect(ui->splitOutputBtn, &QToolButton::clicked, this, &MainWindow::toggleOutputSplitter);

    // load search widget and hide it
    QUiLoader loader;
    {
        QFile file(":/searchwidget.ui");
        file.open(QFile::ReadOnly);
        search_widget = loader.load(&file, ui->mainOutput);
        search_input = search_widget->findChild<QLineEdit*>("searchInput");
        search_prev_button = search_widget->findChild<QToolButton*>("previousButton");
        search_next_button = search_widget->findChild<QToolButton*>("nextButton");
    }
    search_widget->hide();

    // create the search results highlighter for main output
    SearchHighlighter *search_highlighter_main = new SearchHighlighter(ui->mainOutput->document());
    connect(search_input, &QLineEdit::textChanged, search_highlighter_main, &SearchHighlighter::setSearchString);

    // search result highlighter (without search cursor) for bottom output
    SearchHighlighter *search_highlighter_bottom = new SearchHighlighter(ui->bottomOutput->document(), false);
    connect(search_input, &QLineEdit::textChanged, search_highlighter_bottom, &SearchHighlighter::setSearchString);

    // connect search-related signals/slots
    connect(search_prev_button, &QToolButton::clicked,
	search_highlighter_main, &SearchHighlighter::previousOccurence);
    connect(search_next_button, &QToolButton::clicked,
	search_highlighter_main, &SearchHighlighter::nextOccurence);
    connect(search_highlighter_main, &SearchHighlighter::cursorPosChanged,
	this, &MainWindow::handleCursosPosChanged);
    connect(search_highlighter_main, &SearchHighlighter::totalOccurencesChanged,
	this, &MainWindow::handleTotalOccurencesChanged);
    connect(ui->searchButton, &QToolButton::toggled, this, &MainWindow::showSearchWidget);

    // additional configuration for bottom output
    ui->bottomOutput->hide();
    ui->bottomOutput->document()->setMaximumBlockCount(MAX_OUTPUT_LINES);

    // populate file transfer protocol combobox
    ui->protocolCombo->addItem("XModem", SessionManager::XMODEM);
    ui->protocolCombo->addItem("YModem", SessionManager::YMODEM);
    ui->protocolCombo->addItem("ZModem", SessionManager::ZMODEM);

    // transfer file over XModem protocol
    connect(ui->fileTransferButton, &QToolButton::clicked, this, &MainWindow::handleFileTransfer);
    connect(session_mgr, &SessionManager::fileTransferEnded, this, &MainWindow::handleFileTransferEnded);

    // fill end of line chars combobox
    ui->eolCombo->addItem(QStringLiteral("CR \'\\r\'"), CR);
    ui->eolCombo->addItem(QStringLiteral("LF \'\\n\'"), LF);
    ui->eolCombo->addItem(QStringLiteral("CR+LF \'\\r\\n\'"), CRLF);
    ui->eolCombo->addItem(QStringLiteral("None"), None);
    ui->eolCombo->currentData(LF);
    _end_of_line = QByteArray("\n", 1);

    connect(ui->eolCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::handleEOLCharChanged);

    // install event filters
    ui->mainOutput->viewport()->installEventFilter(this);
    ui->bottomOutput->viewport()->installEventFilter(this);
    search_input->installEventFilter(this);
    installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleSessionOpened()
{
    // clear output buffer
    output_mgr->clear();

    // clear both output windows
    ui->mainOutput->clear();
    ui->bottomOutput->clear();

    ui->connectButton->setDisabled(true);
    ui->disconnectButton->setEnabled(true);

    // enable file transfer
    ui->fileTransferButton->setEnabled(true);
}

void MainWindow::handleSessionClosed()
{
    ui->connectButton->setEnabled(true);
    ui->disconnectButton->setDisabled(true);

    // disable file transfer
    ui->fileTransferButton->setDisabled(true);
}

void MainWindow::handleFileTransfer()
{
    QString filename = QFileDialog::getOpenFileName(
                this, QStringLiteral("Select file to send transfer"));

    if (filename.isNull())
        return;

    Q_ASSERT_X(progress_dialog == 0, "MainWindow::handleFileTransfer()", "progress_dialog should be null");

    // display a progress dialog
    progress_dialog = new QProgressDialog(this);
    connect(progress_dialog, &QProgressDialog::canceled,
            session_mgr, &SessionManager::handleTransferCancelledByUser);

    progress_dialog->setRange(0, 100);
    progress_dialog->setWindowModality(Qt::ApplicationModal);
    progress_dialog->setLabelText(
                QStringLiteral("Initiating connection with receiver"));

    // update progress dialog
    connect(session_mgr, &SessionManager::fileTransferProgressed,
            this, &MainWindow::handleFileTransferProgressed);

    int protocol = ui->protocolCombo->currentData().toInt();
    session_mgr->transferFile(filename,
        static_cast<SessionManager::Protocol>(protocol));

    // disable UI elements acting on QSerialPort instance, as long as
    // objectds involved in FileTransferred are not destroyed or back
    // to their pre-file-transfer state
    ui->fileTransferButton->setEnabled(false);
    ui->disconnectButton->setEnabled(false);
    ui->inputBox->setEnabled(false);

    // progress dialog event loop
    progress_dialog->exec();

    delete progress_dialog;
    progress_dialog = 0;
}

void MainWindow::handleFileTransferProgressed(int percent)
{
    if (progress_dialog)
    {
        progress_dialog->setValue(percent);
        progress_dialog->setLabelText(QStringLiteral("Transferring file"));
    }
}

void MainWindow::handleFileTransferEnded(FileTransfer::TransferError error)
{
    switch (error)
    {
        case FileTransfer::LocalCancelledError:
            break;
        case FileTransfer::NoError:
            QMessageBox::information(this, tr("Cutecom-ng"), QStringLiteral("File transferred successfully"));
            break;
        default:
            progress_dialog->setLabelText(FileTransfer::errorString(error));
            break;
    }

    // re-enable UI elements acting on QSerialPort instance
    ui->fileTransferButton->setEnabled(true);
    ui->disconnectButton->setEnabled(true);
    ui->inputBox->setEnabled(true);
}

void MainWindow::handleNewInput(QString entry)
{
    // if session is not open, this also keeps user input
    if (session_mgr->isSessionOpen())
    {
        QByteArray to_send(entry.toLocal8Bit());
        to_send.append(_end_of_line);
        session_mgr->sendToSerial(to_send);
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
    if (event->type() == QEvent::Resize && target == ui->mainOutput->viewport())
    {
        // re position search widget when main output inner size changes
        // this takes into account existence of vertical scrollbar
        QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event);
        search_widget->move(resizeEvent->size().width() - search_widget->width(), 0);
    }
    else if (event->type() == QEvent::Wheel)
    {
        // mouse wheel allowed only for main output in 'browsing mode'
        if (target == ui->bottomOutput->viewport() ||
                (target == ui->mainOutput->viewport() && !ui->bottomOutput->isVisible()))
            return true;
    }
    else if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (ui->searchButton->isChecked())
        {
            // hide search widget on Escape key press
            if (keyEvent->key() == Qt::Key_Escape)
                ui->searchButton->toggle();
        }
        else
        {
            // show search widget on Ctrl-F
            if (keyEvent->key() == Qt::Key_F && keyEvent->modifiers() == Qt::ControlModifier)
                ui->searchButton->toggle();
        }
        if (target == search_input)
        {
            if (keyEvent->key() == Qt::Key_Return)
            {
                if (keyEvent->modifiers() == Qt::ShiftModifier)
                    search_prev_button->click();
                else
                    search_next_button->click();
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
    // move cursor
    QTextCursor text_cursor = ui->mainOutput->textCursor();
    text_cursor.setPosition(pos);

    // ensure search result cursor is visible
    ui->mainOutput->ensureCursorVisible();
    ui->mainOutput->setTextCursor(text_cursor);
}

void MainWindow::handleTotalOccurencesChanged(int total_occurences)
{
    if (total_occurences == 0)
        search_input->setStyleSheet("QLineEdit{background-color: red;}");
    else
        search_input->setStyleSheet("");
}

void MainWindow::handleEOLCharChanged(int index)
{
    Q_UNUSED(index)

    switch(ui->eolCombo->currentData().toInt())
    {
        case CR:
            _end_of_line = QByteArray("\r", 1);
            break;
        case LF:
            _end_of_line = QByteArray("\n", 1);
            break;
        case CRLF:
            _end_of_line = QByteArray("\r\n", 2);
            break;
        case None:
            _end_of_line.clear();
            break;
        default:
            Q_ASSERT_X(false, "MainWindow::handleEOLCharChanged",
                       "unknown EOL char value: " + ui->eolCombo->currentData().toInt());
            break;
    }
}
