#ifndef SLIDEVPANEL_H
#define SLIDEVPANEL_H

#include "slidepanel.h"

class SlideVPanel : public SlidePanel {
    Q_OBJECT
public:
    explicit SlideVPanel(QWidget *w);
    ~SlideVPanel();
    void setPosition(PanelVPosition);
    void containerResized(QSize parentSz);
    QSize triggerSize();

protected:
    PanelVPosition position;
    virtual void paintEvent(QPaintEvent* event);
    void recalculateGeometry();
};

#endif // SLIDEVPANEL_H
