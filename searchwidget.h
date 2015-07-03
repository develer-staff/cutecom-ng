/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Search widget header
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>

namespace Ui {
class SearchWidget;
}

/**
 * \brief The SearchWidget class
 */
class SearchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SearchWidget(QWidget *parent = 0);
    ~SearchWidget();

private:
    Ui::SearchWidget *ui;

signals:

public slots:
};

#endif // SEARCHWIDGET_H
