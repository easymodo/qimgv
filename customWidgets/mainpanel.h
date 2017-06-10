#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QVBoxLayout>
#include "slidehpanel.h"
#include "../customWidgets/iconbutton.h"

class MainPanel : public SlideHPanel {
    Q_OBJECT
public:
    MainPanel(QWidget *parent);
    void setPosition(PanelHPosition);
    void setWidget(QWidget* w);

private slots:
    void readSettings();

private:
    QVBoxLayout buttonsLayout;
    QWidget buttonsWidget;
    IconButton openButton, exitFullscreenButton, settingsButton, exitButton;

protected:
    virtual void paintEvent(QPaintEvent* event);
};

#endif // MAINPANEL_H
