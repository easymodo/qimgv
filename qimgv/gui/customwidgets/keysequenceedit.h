#pragma once

#include <QPushButton>
#include "shortcutbuilder.h"
#include "utils/actions.h"

class KeySequenceEdit : public QPushButton {
    Q_OBJECT
public:
    KeySequenceEdit(QWidget *parent);
    QString sequence();    
    void setText(const QString &text);

protected:
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);

    void mouseReleaseEvent(QMouseEvent *e);
    bool focusNextPrevChild(bool);
private:
    void processEvent(QEvent *e);
    QString mSequence;

signals:
    void edited();
};
