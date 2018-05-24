#include "containerwidget.h"

ContainerWidget::ContainerWidget(QWidget *parent) : QWidget(parent) {
    layout.setContentsMargins(0, 0, 0, 0);
    this->setLayout(&layout);
}

ContainerWidget::ContainerWidget(std::shared_ptr<QWidget> _contentWidget, QWidget *parent) : ContainerWidget(parent) {
    setContentWidget(_contentWidget);
}

void ContainerWidget::setContentWidget(std::shared_ptr<QWidget> w) {
    removeContentWidget();
    mContentWidget = w;
    layout.addWidget(w.get());
}

void ContainerWidget::removeContentWidget() {
    if(mContentWidget) {
        layout.removeWidget(mContentWidget.get());
        mContentWidget.reset();
    }
}

std::shared_ptr<QWidget> ContainerWidget::contentWidget() {
    return mContentWidget;
}


void ContainerWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    emit resized(size());
}
