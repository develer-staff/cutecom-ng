#ifndef INPUTLINE_H
#define INPUTLINE_H

#include <QLineEdit>
#include <QKeyEvent>

class InputLine : public QLineEdit
{
    Q_OBJECT

public:
    InputLine(QWidget *parent = 0);

signals:
    void lineSent(const QByteArray &data);

protected:
    virtual void keyPressEvent(QKeyEvent *e);
};

#endif // INPUTLINE_H
