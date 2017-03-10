#ifndef SLIDEHPANEL_H
#define SLIDEHPANEL_H

#include "slidepanel.h"

class SlideHPanel : public SlidePanel
{
    Q_OBJECT
public:
    explicit SlideHPanel(QWidget *w, QWidget *parent = 0);
    ~SlideHPanel();
    void updatePanelPosition();
    void setPosition(PanelHPosition);

private:
    PanelHPosition position;

public slots:
    void parentResized(QSize parentSize);
private slots:
    void readSettings();
};

#endif // SLIDEHPANEL_H
