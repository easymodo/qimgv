#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QVBoxLayout>
#include "gui/customwidgets/slidehpanel.h"
#include "gui/customwidgets/iconbutton.h"

class MainPanel : public SlideHPanel {
    Q_OBJECT
public:
    MainPanel(QWidget *parent);
    void setPosition(PanelHPosition);
    void setWidget(QWidget* w);
    void setWindowButtonsEnabled(bool mode);

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
