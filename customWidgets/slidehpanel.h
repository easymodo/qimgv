#ifndef SLIDEHPANEL_H
#define SLIDEHPANEL_H

#include "slidepanel.h"

class SlideHPanel : public SlidePanel
{
    Q_OBJECT
public:
    explicit SlideHPanel(QWidget *parent);
    ~SlideHPanel();
    void setPosition(PanelHPosition);
    void containerResized(QSize parentSz);
    QSize triggerSize();
    void setPanelHeight(int);

protected:
    PanelHPosition position;
    int invisibleMargin;
    int panelHeight;
    void recalculateGeometry();
};

#endif // SLIDEHPANEL_H
