#pragma once

#include <QVBoxLayout>
#include "gui/customwidgets/slidehpanel.h"
#include "gui/customwidgets/actionbutton.h"
#include "gui/panels/mainpanel/thumbnailstrip.h"

class MainPanel : public SlideHPanel {
    Q_OBJECT
public:
    MainPanel(FloatingWidgetContainer *parent);
    ~MainPanel();
    void setHeight(int newHeight);
    void setPosition(PanelHPosition);
    void setExitButtonEnabled(bool mode);
    std::shared_ptr<DirectoryViewWrapper> getWrapper();

private slots:
    void readSettings();

private:
    QVBoxLayout buttonsLayout;
    QWidget buttonsWidget;
    std::shared_ptr<ThumbnailStrip> thumbnailStrip;
    ActionButton *openButton, *settingsButton, *exitButton, *folderViewButton;

protected:
    virtual void paintEvent(QPaintEvent* event);
};
