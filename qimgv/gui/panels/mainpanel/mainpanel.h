#pragma once

#include <QVBoxLayout>
#include "gui/customwidgets/slidehpanel.h"
#include "gui/customwidgets/actionbutton.h"
#include "gui/panels/mainpanel/thumbnailstripproxy.h"

class MainPanel : public SlideHPanel {
    Q_OBJECT
public:
    MainPanel(FloatingWidgetContainer *parent);
    ~MainPanel();
    void setHeight(int newHeight);
    void setPosition(PanelHPosition);
    void setExitButtonEnabled(bool mode);
    std::shared_ptr<ThumbnailStripProxy> getThumbnailStrip();

    void setupThumbnailStrip();
private slots:
    void readSettings();

private:
    QVBoxLayout buttonsLayout;
    QWidget buttonsWidget;
    std::shared_ptr<ThumbnailStripProxy> thumbnailStrip;
    ActionButton *openButton, *settingsButton, *exitButton, *folderViewButton;

protected:
    virtual void paintEvent(QPaintEvent* event);
};
