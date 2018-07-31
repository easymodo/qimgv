#ifndef KEYSEQUENCEEDIT_H
#define KEYSEQUENCEEDIT_H

#include <QPushButton>
#include "shortcutbuilder.h"
#include "utils/actions.h"

class KeySequenceEdit : public QPushButton {
    Q_OBJECT
public:
    KeySequenceEdit(QWidget *parent);
    QString sequence();

protected:
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);

private:
    void processEvent(QEvent *e);
    QString mSequence;

private slots:
    void enterEditMode();
    void exitEditMode();

signals:
    void edited();
};

#endif // KEYSEQUENCEEDIT_H
