/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief InputLine class header
 *
 * \author Aurelien Rainone <aurelien@develer.org>
 */

#ifndef INPUTLINE_H
#define INPUTLINE_H

#include <QLineEdit>
#include <QKeyEvent>

/**
 * \brief QLineEdit customized for command line behaviour
 *
 * QLineEdit::returnPressed signal does not pass the text as parameter
 * InputLine::commandSent signal does that
 */
class InputLine : public QLineEdit
{
    Q_OBJECT

private:

    /**
     * \brief default line endings, appended to each typed command
     */
    QString _line_end;

public:
    InputLine(QWidget *parent = 0);

signals:
    /**
     * \brief signal emitted after Enter or Return key presses
     * \param command    line typed in the edition box, concatenated with a line ending
     */
    void commandSent(const QByteArray &command);

protected:

    /**
     * \brief intercept Return and Enter keys in order to emit commandSent signal
     *
     * intercept Return and Enter keys in order to emit commandSent signal
     *
     * \param e     key press event
     */
    virtual void keyPressEvent(QKeyEvent *e);
};

#endif // INPUTLINE_H
