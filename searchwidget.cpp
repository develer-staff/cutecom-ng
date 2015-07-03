/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Search widget implementation
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#include "searchwidget.h"
#include "ui_searchwidget.h"
#include <QDebug>

SearchWidget::SearchWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::SearchWidget)
{
    ui->setupUi(this);
    qDebug() << "SearchWidget::SearchWidget(" << parent << ")";
}


SearchWidget::~SearchWidget()
{
    delete ui;
}
