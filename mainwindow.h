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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextCharFormat>
#include <QMap>

namespace Ui {
class MainWindow;
}

class SessionManager;
class OutputManager;
class ConnectDialog;
class QLineEdit;
class SearchHighlighter;

/**
 * \brief main cutecom-ng window
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow      *ui;
    SessionManager      *session_mgr;
    OutputManager       *output_mgr;
    ConnectDialog       *connect_dlg;
    QWidget             *search_widget;
    QLineEdit           *search_input;
    SearchHighlighter   *search_highlighter;

    /**
     * \brief pair {position ; length}
     * (used for keeping track of highlighted search results)
     */
    typedef QPair<int, int> position_length_type;

    /**
     * \brief currently highlighted search results
     * - key : search text position
     * - value : text char format before being highlighted
     */
    QMap<position_length_type, QTextCharFormat> search_results;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:

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
};

#endif // MAINWINDOW_H
