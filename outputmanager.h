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
 * \brief handle output data
 */
class OutputManager : public QObject
{
    Q_OBJECT

private:

    /// data received in current session (concatenated)
    QByteArray _buffer;

public:
    explicit OutputManager(QObject *parent = 0);

    /**
     * \brief retrieve internal buffer
     */
    const QByteArray& buffer();

    /**
     * \brief clear internal buffer
     */
    void clear();

    /**
     * \brief handle new data
     * append new data to the internal buffer
     * and emit dataConverted signal
     */
    void operator << (const QByteArray &data);

signals:

    void dataConverted(const QString & data);
};

#endif // OUTPUTMANAGER_H
