#pragma once

#include <QWidget>
#include "gui/customwidgets/overlaywidget.h"

namespace Ui {
class InfoOverlay;
}

class InfoOverlay : public OverlayWidget {
    Q_OBJECT

public:
    explicit InfoOverlay(FloatingWidgetContainer *parent = nullptr);
    ~InfoOverlay();

    void setInfo(QString pos, QString fileName, QString info);
private:
    Ui::InfoOverlay *ui;
};
