/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief HistoryComboBox class header
 *
 * \author Aurelien Rainone <aurelien@develer.com>
 */

#ifndef HISTORYCOMBOBOX_H
#define HISTORYCOMBOBOX_H

#include <QComboBox>
#include <QKeyEvent>

class History;

/**
 * \brief combobox customized to act as an history
 */
class HistoryComboBox : public QComboBox
{
    Q_OBJECT

private:
    History *history;

public:
    HistoryComboBox(QWidget *parent = 0);

signals:
    void lineEntered(const QString);

protected:
    virtual void keyPressEvent(QKeyEvent *e);

private:
    void fillList(QString current_text);
};


#endif // HISTORYCOMBOBOX_H
