/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief OutputManager class header
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H

#include <QObject>
#include <QByteArray>

class QTextEdit;

/**
 * \brief handle display and saving of serial port output
 */
class OutputManager : public QObject
{
    Q_OBJECT

private:

    /// contains data appended to the output
    QByteArray all_data;
    QTextEdit *edit;

public:
    explicit OutputManager(QTextEdit *textedit, QObject *parent = 0);

signals:

public slots:

    /**
     * /brief append new data
     * /param data array
     */
    void appendData(QByteArray & data);
};

#endif // OUTPUTMANAGER_H
