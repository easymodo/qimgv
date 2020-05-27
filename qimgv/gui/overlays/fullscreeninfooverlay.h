#pragma once

#include <QWidget>
#include "gui/customwidgets/overlaywidget.h"

namespace Ui {
class FullscreenInfoOverlay;
}

class FullscreenInfoOverlay : public OverlayWidget {
    Q_OBJECT

public:
    explicit FullscreenInfoOverlay(FloatingWidgetContainer *parent = nullptr);
    ~FullscreenInfoOverlay();
    void setInfo(QString pos, QString fileName, QString info);

private:
    Ui::FullscreenInfoOverlay *ui;
};
