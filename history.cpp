/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief History class implementation
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#include "history.h"

History::History(QObject *parent)
    : QObject(parent),
      max_entries(5)
{
    current = -1;
}

void History::add(const QString &entry)
{
    // remove eventual duplicate of new entry
    int dup = history.indexOf(entry);
    if (dup >= 0)
        history.removeAt(dup);

    history.prepend(entry);

    // remove extra elements
    if (history.size() > max_entries)
        history.pop_back();

    current = -1;
}

const QString History::previous()
{
    QString prev("");
    if (history.size() > 0)
    {
        if (current == -1)
            current = history.size() - 1;
        else if (current > 0)
            --current;
        prev = history[current];
    }
   return prev;
}

const QString History::next()
{
    QString nex("");
    if (history.size() > 0)
    {
        if (current == -1)
            current = 0;
        else if (current < history.size() - 1)
            ++current;
        nex = history[current];
    }
    return nex;
}

const QStringList& History::getHistory() const
{
    return history;
}
