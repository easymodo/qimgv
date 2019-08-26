#ifndef INFOOVERLAY_H
#define INFOOVERLAY_H

#include <QWidget>
#include "gui/customwidgets/floatingwidget.h"

namespace Ui {
class InfoOverlay;
}

class InfoOverlay : public FloatingWidget {
    Q_OBJECT

public:
    explicit InfoOverlay(OverlayContainerWidget *parent = nullptr);
    ~InfoOverlay();

    void setInfo(QString pos, QString fileName, QString info);
private:
    Ui::InfoOverlay *ui;
};

#endif // INFOOVERLAY_H
