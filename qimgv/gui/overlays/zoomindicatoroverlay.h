#pragma once

#include "gui/customwidgets/overlaywidget.h"
#include <QApplication>
#include <QLabel>
#include <QTimer>
#include <QHBoxLayout>

class ZoomIndicatorOverlay : public OverlayWidget {
    Q_OBJECT
public:
    explicit ZoomIndicatorOverlay(FloatingWidgetContainer *parent = nullptr);

    void setScale(qreal scale);
    void show();
    void show(int duration);
protected:
    virtual void recalculateGeometry();

private:
    QFontMetrics *fm;
    QHBoxLayout layout;
    QLabel label;
    QTimer visibilityTimer;
    int hideDelay;
};

