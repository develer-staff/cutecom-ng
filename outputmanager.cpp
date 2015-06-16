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

#include "outputmanager.h"

#include <QTextEdit>

OutputManager::OutputManager(QTextEdit *textedit, QObject *parent) : QObject(parent), edit(textedit)
{

}

void OutputManager::appendData(QByteArray & data)
{
    all_data.append(data);

    edit->insertPlainText (data);
}

