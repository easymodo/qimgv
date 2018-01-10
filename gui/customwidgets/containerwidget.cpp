#include "containerwidget.h"

ContainerWidget::ContainerWidget(QWidget *parent) : QWidget(parent)
{

}

void ContainerWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    emit resized(size());
}
