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

namespace Ui {
class MainWindow;
}


class SessionManager;
class OutputManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    SessionManager *session_mgr;
    OutputManager *output_mgr;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:

    /**
     * /brief handle return
     */
    void handleNewInput(QString entry);

    /**
     * /brief show the connection dialog
     */
    void openConnectionDialog();

    /**
     * /brief add data to the output view
     */
    void addDataToView(const QString & textdata);


    void handleCurrentIndexChanged(int index);
};

#endif // MAINWINDOW_H
