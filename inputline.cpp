#include "inputline.h"

InputLine::InputLine(QWidget *parent) :
    QLineEdit(parent)
{

}

void InputLine::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Return:
        case Qt::Key_Enter:
        {
            QString line = text() + QString("\r\n");
            emit lineSent(line.toLocal8Bit());
            clear();
            break;
        }
        default:
            QLineEdit::keyPressEvent(e);
            break;
    }
}

