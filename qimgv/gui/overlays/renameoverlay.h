#pragma once

#include <QPushButton>
#include <QLineEdit>
#include <QKeyEvent>
#include <QTimer>

#include "gui/customwidgets/overlaywidget.h"
#include "settings.h"

namespace Ui {
class RenameOverlay;
}

class RenameOverlay : public OverlayWidget
{
    Q_OBJECT

public:
    explicit RenameOverlay(FloatingWidgetContainer *parent);
    ~RenameOverlay();

public slots:
    void setName(QString name);
    void show();
    void hide();
signals:
    void renameRequested(QString name);

protected:
    void keyPressEvent(QKeyEvent *event);
private slots:
    void rename();
    void onCancel();

private:
    Ui::RenameOverlay *ui;
    QString origName;
    void selectName();
};
