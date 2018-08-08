#ifndef SLIDEVPANEL_H
#define SLIDEVPANEL_H

#include "slidepanel.h"

class SlideVPanel : public SlidePanel {
    Q_OBJECT
public:
    explicit SlideVPanel(OverlayContainerWidget *w);
    ~SlideVPanel();
    void setPosition(PanelVPosition);
    QRect triggerRect();

protected:
    PanelVPosition position;
    void recalculateGeometry();
    virtual void updateTriggerRect();
};

#endif // SLIDEVPANEL_H
