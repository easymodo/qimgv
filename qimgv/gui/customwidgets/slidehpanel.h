/* This panel will use sizeHint().height() from the widget it contains.
 * Width will match containerSize().width()
 */

#pragma once

#include "slidepanel.h"

class SlideHPanel : public SlidePanel
{
    Q_OBJECT
public:
    explicit SlideHPanel(FloatingWidgetContainer *parent);
    ~SlideHPanel();
    void setPosition(PanelHPosition);
    QRect triggerRect();
    void setPanelHeight(int);

protected:
    PanelHPosition position;
    int bottomMargin;
    int panelHeight;
    void recalculateGeometry();
    virtual void updateTriggerRect();
};
