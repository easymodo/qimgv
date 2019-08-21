#include "documentwidget.h"

DocumentWidget::DocumentWidget(std::shared_ptr<ViewerWidget> viewWidget, std::shared_ptr<InfoBar> infoBar, QWidget *parent)
    : OverlayContainerWidget(parent)
{
    layout.setContentsMargins(0,0,0,0);
    layout.setSpacing(0);
    setLayout(&layout);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMouseTracking(true);
    mViewWidget = viewWidget;
    mViewWidget->setParent(this);
    layout.addWidget(mViewWidget.get());
    mViewWidget.get()->show();
    mInfoBar = infoBar;
    mInfoBar->setParent(this);
    layout.addWidget(mInfoBar.get());
}

std::shared_ptr<ViewerWidget> DocumentWidget::viewWidget() {
    return mViewWidget;
}
