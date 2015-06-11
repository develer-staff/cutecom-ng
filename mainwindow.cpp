#include "mainwindow.h"
#include "connectdialog.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->connectButton, &QAbstractButton::clicked, this, &MainWindow::openNewConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openNewConnection()
{
    ConnectDialog dialog(this);
    dialog.exec();
}

