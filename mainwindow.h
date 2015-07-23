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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "filetransfer.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class SessionManager;
class OutputManager;
class ConnectDialog;
class QLineEdit;
class QToolButton;
class QProgressDialog;

/**
 * \brief main cutecom-ng window
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    /**
     * \brief different end of line char(s) that can be appended
     * to every line sent to the serial port
     */
    enum EndOfLineChar
    {
        None    = 0,    /// none, line is sent as-is
        LF      = 1,    /// LineFeed:       [0x0a]
        CR      = 2,    /// CarriageReturn: [0x0d]
        CRLF    = 3     /// CR + LF:        [0x0d, 0x0a]
    };

private:
    Ui::MainWindow      *ui;
    SessionManager      *session_mgr;
    OutputManager       *output_mgr;
    ConnectDialog       *connect_dlg;
    QWidget             *search_widget;
    QLineEdit           *search_input;
    QToolButton         *search_prev_button;
    QToolButton         *search_next_button;
    QProgressDialog     *progress_dialog;
    QByteArray          _end_of_line;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:

    /**
     * \brief handle sessionOpened signal
     */
    void handleSessionOpened();

    /**
     * \brief handle sessionClosed signal
     */
    void handleSessionClosed();

    /**
     * \brief handle buttonClicked on the x/y/zmodem buttons
     * \param type
     */
    void handleFileTransfer();

    /**
     * \brief handle new input
     */
    void handleNewInput(QString entry);

    /**
     * \brief add data to the output view
     */
    void addDataToView(const QString & textdata);

    /**
     * \brief handle arrival of new data
     */
    void handleDataReceived(const QByteArray &data);

    /**
     * \brief toggle bottom output text window and splitter
     */
    void toggleOutputSplitter();

    /**
     * \brief event filter
     */
    bool eventFilter(QObject *target, QEvent *event);

    /**
     * \brief show search widget
     * \param show show or hide?
     */
    void showSearchWidget(bool show);

    /**
     * \brief handle search result cursor position changes
     * \param pos pos of search result cursor
     */
    void handleCursosPosChanged(int pos);

    /**
     * \brief handle changes of number of search string occurences
     * \param total_occurences
     */
    void handleTotalOccurencesChanged(int total_occurences);

    /**
     * \brief handle end of file transfer
     * \param error transfer error code
     */
    void handleFileTransferEnded(FileTransfer::TransferError error);

    /**
     * \brief handle fileTransferProgressed signal
     * \param percent transfer current progression
     */
    void handleFileTransferProgressed(int percent);

    /**
     * \brief handle currentIndexChanged for end of line char combobox
     * \param index index of selected item
     */
    void handleEOLCharChanged(int index);
};

#endif // MAINWINDOW_H
