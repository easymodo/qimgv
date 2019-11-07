#pragma once

#include <QVBoxLayout>
#include "gui/customwidgets/slidehpanel.h"
#include "gui/customwidgets/actionbutton.h"

class MainPanel : public SlideHPanel {
    Q_OBJECT
public:
    MainPanel(std::shared_ptr<QWidget> widget, FloatingWidgetContainer *parent);
    ~MainPanel();
    void setHeight(int newHeight);
    void setPosition(PanelHPosition);
    void setExitButtonEnabled(bool mode);

private slots:
    void readSettings();

private:
    QVBoxLayout buttonsLayout;
    QWidget buttonsWidget;
    ActionButton *openButton, *settingsButton, *exitButton, *folderViewButton;

protected:
    virtual void paintEvent(QPaintEvent* event);
};
