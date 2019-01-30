#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QVBoxLayout>
#include "gui/customwidgets/slidehpanel.h"
#include "gui/customwidgets/actionbutton.h"

class MainPanel : public SlideHPanel {
    Q_OBJECT
public:
    MainPanel(std::shared_ptr<QWidget> widget, OverlayContainerWidget *parent);
    void setHeight(int newHeight);
    void setPosition(PanelHPosition);
    void setWindowButtonsEnabled(bool mode);

private slots:
    void readSettings();

private:
    QVBoxLayout buttonsLayout;
    QWidget buttonsWidget;
    ActionButton *openButton, *exitFullscreenButton, *settingsButton, *exitButton, *folderViewButton;

protected:
    virtual void paintEvent(QPaintEvent* event);
};

#endif // MAINPANEL_H
