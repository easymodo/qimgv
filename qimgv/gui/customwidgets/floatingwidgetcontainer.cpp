#include "floatingwidgetcontainer.h"
#include "gui/customwidgets/floatingwidget.h"

FloatingWidgetContainer::FloatingWidgetContainer(QWidget *parent) : QWidget(parent) {
    // set focus policy so we'll receive focusInEvents
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
}

void FloatingWidgetContainer::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    emit resized(size());
}

// give focus to the first visible overlay that wants it
// ideally this should be a stack with the most recently shown widget at the top
// you are not likely to have >2 overlays at the same time so this suffices
void FloatingWidgetContainer::focusInEvent(QFocusEvent *event) {
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
