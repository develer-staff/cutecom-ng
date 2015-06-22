/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief History class header
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#ifndef HISTORY_H
#define HISTORY_H

#include <QStringList>

/**
 * \brief Manage a fixed-size list of strings (readline-like history)
 */
class History : public QObject
{
    Q_OBJECT

private:
    /// full history
    QStringList history;

    /// maximum size
    const int max_entries;

    /// current element in history
    int current;

public:
    explicit History(QObject *parent = 0);

public:

    /**
     * \brief append a new element to history
     */
    void add(const QString &entry);

    /**
     * \brief get previous element (regarding current one)
     */
    const QString previous();

    /**
     * \brief get next element (regarding current one)
     */
    const QString next();

    /**
     * \brief get all history elements
     */
    const QStringList& getHistory() const;
};

#endif // HISTORY_H
