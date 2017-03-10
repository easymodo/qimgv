#ifndef SLIDEVPANEL_H
#define SLIDEVPANEL_H

#include "slidepanel.h"

class SlideVPanel : public SlidePanel
{
    Q_OBJECT
public:
    explicit SlideVPanel(QWidget *w, QWidget *parent = 0);
    ~SlideVPanel();
    void updatePanelPosition();
    void setPosition(PanelVPosition);

private:
    PanelVPosition position;

public slots:
    void parentResized(QSize parentSize);

protected:
    virtual void paintEvent(QPaintEvent* event);
};

#endif // SLIDEVPANEL_H
