// this spin box ignores all keyboard input except numbers, up/down keys

#ifndef SPINBOXINPUTFIX_H
#define SPINBOXINPUTFIX_H

#include <QSpinBox>
#include <QKeyEvent>
#include <QDebug>
#include "components/actionmanager/actionmanager.h"

class SpinBoxInputFix : public QSpinBox
{
    Q_OBJECT
public:
    SpinBoxInputFix(QWidget *parent = nullptr);

private:
    QStringList allowedKeys;

protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // SPINBOXINPUTFIX_H
