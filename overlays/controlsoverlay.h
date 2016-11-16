#ifndef ControlsOverlay_H
#define ControlsOverlay_H
#include "../customWidgets/clickablelabel.h"
#include <QDebug>

class ControlsOverlay : public ClickableLabel
{
    Q_OBJECT
public:
    explicit ControlsOverlay(QWidget *parent = 0);
    void updatePosition(QSize containerSz);

signals:
    void exitClicked();
};

#endif // ControlsOverlay_H
