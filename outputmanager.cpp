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

OutputManager::OutputManager(QObject *parent) : QObject(parent)
{

}

void OutputManager::handleNewData(const QByteArray &data)
{
    // append raw data to the session buffer
    _buffer.append(data);

    // notify that we have new data (in display format)
    emit dataConverted(QString::fromLocal8Bit(data));
}

const QByteArray& OutputManager::buffer()
{
    return _buffer;
}


void OutputManager::clear()
{
    _buffer.clear();
}
