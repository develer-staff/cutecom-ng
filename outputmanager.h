/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief handle display and saving of serial port output
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H

#include <QObject>
#include <QByteArray>

class QTextEdit;

class OutputManager : public QObject
{
    Q_OBJECT

private:

    QByteArray all_data;
    QTextEdit *edit;

public:
    explicit OutputManager(QTextEdit *textedit, QObject *parent = 0);

signals:

public slots:
    void appendData(QByteArray & data);
};

#endif // OUTPUTMANAGER_H
