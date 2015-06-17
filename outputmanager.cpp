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

#include <QTextEdit>
#include <QScrollBar>

OutputManager::OutputManager(QTextEdit *textedit, QObject *parent) : QObject(parent), edit(textedit)
{

}

void OutputManager::appendData(QByteArray & data)
{
    // append raw data to the session buffer
    all_data.append(data);

    // save current text selection
    QTextCursor cursor = edit->textCursor();

    // insert text at end of 'edit' (this clears any selection)
    edit->moveCursor(QTextCursor::End);
    edit->insertPlainText (QString::fromLocal8Bit(data).remove('\n'));

    // revert text selection
    edit->setTextCursor(cursor);

    // push scroll to the bottom
    QScrollBar *vbar = edit->verticalScrollBar();
    vbar->setValue(vbar->maximum());
}

