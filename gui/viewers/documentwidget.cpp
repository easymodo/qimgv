#include "documentwidget.h"

DocumentWidget::DocumentWidget(QWidget *parent) : OverlayContainerWidget(parent) {
    layout.setContentsMargins(0,0,0,0);
    setLayout(&layout);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMouseTracking(true);
}

DocumentWidget::DocumentWidget(std::shared_ptr<QWidget> _viewWidget, QWidget *parent)
    : DocumentWidget(parent)
{
    setViewWidget(_viewWidget);
}

void DocumentWidget::setViewWidget(std::shared_ptr<QWidget> _viewWidget) {
    if(mViewWidget) {
        layout.removeWidget(mViewWidget.get());
    }
    mViewWidget = _viewWidget;
    mViewWidget->setParent(this);
    layout.addWidget(mViewWidget.get());
    mViewWidget.get()->show();
}

std::shared_ptr<QWidget> DocumentWidget::viewWidget() {
    return mViewWidget;
}
