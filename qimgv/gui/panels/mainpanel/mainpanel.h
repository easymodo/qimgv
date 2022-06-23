#pragma once

#include <QVBoxLayout>
#include "gui/customwidgets/slidepanel.h"
#include "gui/customwidgets/actionbutton.h"
#include "gui/panels/mainpanel/thumbnailstripproxy.h"

class MainPanel : public SlidePanel {
    Q_OBJECT
public:
    MainPanel(FloatingWidgetContainer *parent);
    ~MainPanel();
    void setPosition(PanelPosition);
    void setExitButtonEnabled(bool mode);
    std::shared_ptr<ThumbnailStripProxy> getThumbnailStrip();
    void setupThumbnailStrip();
    QSize sizeHint() const;

public slots:
    void readSettings();

signals:
    void pinned(bool mode);

private slots:
    void onPinClicked();

private:
    QVBoxLayout buttonsLayout;
    QWidget buttonsWidget;
    std::shared_ptr<ThumbnailStripProxy> thumbnailStrip;
    ActionButton *openButton, *settingsButton, *exitButton, *folderViewButton, *pinButton;

protected:
    virtual void paintEvent(QPaintEvent* event);
};
