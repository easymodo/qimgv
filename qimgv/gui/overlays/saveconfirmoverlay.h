#ifndef SAVECONFIRMOVERLAY_H
#define SAVECONFIRMOVERLAY_H

#include "gui/customwidgets/overlaywidget.h"
#include "settings.h"
#include <QPushButton>

namespace Ui {
class SaveConfirmOverlay;
}

class SaveConfirmOverlay : public OverlayWidget
{
    Q_OBJECT
public:
    explicit SaveConfirmOverlay(FloatingWidgetContainer *parent = nullptr);
    ~SaveConfirmOverlay();

signals:
    void saveClicked();
    void saveAsClicked();
    void discardClicked();

private slots:
    void readSettings();
private:
    Ui::SaveConfirmOverlay *ui;
};

#endif // SAVECONFIRMOVERLAY_H
