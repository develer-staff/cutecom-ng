#include "inputline.h"

InputLine::InputLine(QWidget *parent)
    :   QLineEdit(parent),
        _line_end("\r\n")
{

}

void InputLine::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Return:
        case Qt::Key_Enter:
        {
            QString line = text() + _line_end;
            emit commandSent(line.toLocal8Bit());
            clear();
            break;
        }
        default:
            QLineEdit::keyPressEvent(e);
            break;
    }
}

