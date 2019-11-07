#include "overlaycontainerwidget.h"
#include "gui/customwidgets/floatingwidget.h"

OverlayContainerWidget::OverlayContainerWidget(QWidget *parent) : QWidget(parent) {
    // set focus policy so we'll receive focusInEvents
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
}

void OverlayContainerWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    emit resized(size());
}

void OverlayContainerWidget::focusInEvent(QFocusEvent *event) {
    QWidget::focusInEvent(event);
    auto children = this->children();
    for (auto i : children) {
        FloatingWidget *overlay = qobject_cast<FloatingWidget*>(i);
        if(overlay && overlay->acceptKeyboardFocus() && overlay->isVisible()) {
            overlay->setFocus();
            break;
        }
    }
}
