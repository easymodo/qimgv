#pragma once

#include <QPushButton>
#include <QLineEdit>
#include <QKeyEvent>
#include <QTimer>

#include "gui/customwidgets/overlaywidget.h"
#include "components/actionmanager/actionmanager.h"
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
    void setBackdropEnabled(bool mode);
    void show();
    void hide();
signals:
    void renameRequested(QString name);

protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void recalculateGeometry();

private slots:
    void rename();
    void onCancel();

private:
    Ui::RenameOverlay *ui;
    bool backdrop = false;
    QString origName;
    QList<QString> keyFilter;
    void selectName();
};
