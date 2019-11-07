/* This panel will use sizeHint().width() from the widget it contains.
 * Height will match containerSize().height()
 */

#pragma once

#include "slidepanel.h"

class SlideVPanel : public SlidePanel {
    Q_OBJECT
public:
    explicit SlideVPanel(FloatingWidgetContainer *w);
    ~SlideVPanel();
    void setPosition(PanelVPosition);
    QRect triggerRect();

protected:
    PanelVPosition position;
    void recalculateGeometry();
    virtual void updateTriggerRect();
};
