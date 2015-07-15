/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief HistoryComboBox class implementation
 *
 * \author Aurelien Rainone <aurelien@develer.com>
 */

#include "historycombobox.h"
#include <history.h>
#include <algorithm>
#include <QLineEdit>


HistoryComboBox::HistoryComboBox(QWidget *parent) :
    QComboBox(parent)
{
    history = new History(this);

    connect(this, static_cast<void (HistoryComboBox::*)(int)>(&HistoryComboBox::activated),
            history, &History::setCurrent);
}

void HistoryComboBox::fillList(QString current_text)
{
    QStringList hist_lines(history->getHistory());

    clear();
    addItems(hist_lines);
    setCurrentIndex(hist_lines.indexOf(current_text));
}

void HistoryComboBox::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Up:
            setCurrentIndex(0);
            setItemText(0, history->previous());
            fillList(this->lineEdit()->text());
            break;

        case Qt::Key_Down:
            setCurrentIndex(0);
            setItemText(0, history->next());
            fillList(this->lineEdit()->text());
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
        {
            QString line = this->lineEdit()->text();
            if (line.length() > 0)
            {
                // don't treat empty input
                history->add(line);
                fillList("");
                emit lineEntered(line);
            }
            break;
        }
        default:
            QComboBox::keyPressEvent(e);
            break;
    }
}
