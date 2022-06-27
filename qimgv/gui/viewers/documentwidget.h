#pragma once

#include <memory>
#include <QBoxLayout>
#include "gui/customwidgets/floatingwidgetcontainer.h"
#include "gui/viewers/viewerwidget.h"
#include "gui/panels/mainpanel/mainpanel.h"
#include "gui/panels/infobar/infobarproxy.h"

// TODO: use a template here?

class DocumentWidget : public FloatingWidgetContainer {
public:
    DocumentWidget(std::shared_ptr<ViewerWidget> viewWidget, std::shared_ptr<InfoBarProxy> infoBar, QWidget* parent = nullptr);
    std::shared_ptr<ViewerWidget> viewWidget();
    std::shared_ptr<ThumbnailStripProxy> thumbPanel();
    void setFocus();
    void hideFloatingPanel();
    void hideFloatingPanel(bool animated);
    void setPanelEnabled(bool mode);
    bool panelEnabled();
    void setupMainPanel();
    void setInteractionEnabled(bool mode);
    void allowPanelInit();

public slots:
    void onFullscreenModeChanged(bool mode);

private slots:
    void setPanelPinned(bool mode);
    bool panelPinned();
    void readSettings();

protected:
    void enterEvent(QEnterEvent *event);
    void leaveEvent(QEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    QBoxLayout *layout, *layoutRoot;
    std::shared_ptr<ViewerWidget> mViewWidget;
    std::shared_ptr<InfoBarProxy> mInfoBar;
    std::shared_ptr<MainPanel> mainPanel;
    bool avoidPanelFlag, mPanelEnabled, mPanelFullscreenOnly, mIsFullscreen, mPanelPinned, mInteractionEnabled, mAllowPanelInit;
};
