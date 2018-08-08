#include "documentwidget.h"

DocumentWidget::DocumentWidget(std::shared_ptr<ViewerWidget> _viewWidget, QWidget *parent)
    : OverlayContainerWidget(parent)
{
    layout.setContentsMargins(0,0,0,0);
    setLayout(&layout);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMouseTracking(true);
    mViewWidget = _viewWidget;
    mViewWidget->setParent(this);
    layout.addWidget(mViewWidget.get());
    mViewWidget.get()->show();
}

std::shared_ptr<ViewerWidget> DocumentWidget::viewWidget() {
    return mViewWidget;
}
