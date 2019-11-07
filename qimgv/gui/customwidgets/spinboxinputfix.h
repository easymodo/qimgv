// this spin box ignores all keyboard input except numbers, up/down keys

#pragma once

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
