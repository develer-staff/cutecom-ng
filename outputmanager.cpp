/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief OutputManager class implementation
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#include "outputmanager.h"

#include <algorithm>
#include <iterator>

OutputManager::OutputManager(QObject *parent) : QObject(parent)
{

}

void OutputManager::operator << (const QByteArray &data)
{
    // problem : if a buffer ending with "\r\n" happens to be cut between '\r' and '\n'
    //           the 2 resulting QString will be interpreted as having 2 line jumps,
    //           instead of 1, if displayed in a QTextEdit.
    //           To avoid this, final '\r' are removed from the received buffer,
    //           and prepended to the next one

    // flag indicating that the previously received buffer ended with CR
    static bool prev_ends_with_CR = false;

    // append raw data to the buffer, untouched
    _buffer.append(data);

    QString to_send;
    if (prev_ends_with_CR)
    {
        // CR was removed at the previous buffer, so now we prepend it
        to_send.append('\r');
        prev_ends_with_CR = false;
    }

    if (data.size() > 0)
    {
        QByteArray::const_iterator end_cit = data.cend();
        if (data.endsWith('\r'))
        {
            // if buffer ends with CR, we don't copy it
            end_cit--;
            prev_ends_with_CR = true;
        }
        std::copy(data.begin(), end_cit, std::back_inserter(to_send));
    }

    // notify that we have new data
    emit dataConverted(to_send);
}

const QByteArray& OutputManager::buffer()
{
    return _buffer;
}


void OutputManager::clear()
{
    _buffer.clear();
}
