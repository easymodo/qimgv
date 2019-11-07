#pragma once

#include "gui/customwidgets/overlaywidget.h"
#include <QWheelEvent>

namespace Ui {
class ImageInfoOverlay;
}

class ImageInfoOverlay : public OverlayWidget
{
    Q_OBJECT

public:
    explicit ImageInfoOverlay(FloatingWidgetContainer *parent = nullptr);
    ~ImageInfoOverlay();
    void setExifInfo(QMap<QString, QString>);

protected:
    void wheelEvent(QWheelEvent *event);
private:
    Ui::ImageInfoOverlay *ui;
};
