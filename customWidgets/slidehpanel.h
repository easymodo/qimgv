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

public slots:
    void parentResized(QSize parentSize);
};

#endif // SLIDEHPANEL_H
