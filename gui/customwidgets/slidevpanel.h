#ifndef SLIDEVPANEL_H
#define SLIDEVPANEL_H

#include "slidepanel.h"

class SlideVPanel : public SlidePanel {
    Q_OBJECT
public:
    explicit SlideVPanel(QWidget *w);
    ~SlideVPanel();
    void setPosition(PanelVPosition);
    QRect triggerRect();

protected:
    PanelVPosition position;
    virtual void paintEvent(QPaintEvent* event);
    void recalculateGeometry();
    virtual void updateTriggerRect();
};

#endif // SLIDEVPANEL_H
