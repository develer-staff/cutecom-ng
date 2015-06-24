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
      max_entries(10)
{
    _current = -1;
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

    _current = -1;
}

const QString History::previous()
{
    QString prev("");
    if (history.size() > 0)
    {
        if (_current == -1)
            _current = history.size() - 1;
        else if (_current > 0)
            --_current;
        prev = history[_current];
    }
   return prev;
}

const QString History::next()
{
    QString nex("");
    if (history.size() > 0)
    {
        if (_current == -1)
            _current = 0;
        else if (_current < history.size() - 1)
            ++_current;
        nex = history[_current];
    }
    return nex;
}

const QStringList& History::getHistory() const
{
    return history;
}

void History::setCurrent(int current_index)
{
    _current = current_index;
}

int History::current() const
{
    return _current;
}
