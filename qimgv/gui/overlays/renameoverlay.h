#ifndef RENAMEOVERLAY_H
#define RENAMEOVERLAY_H

#include <QPushButton>
#include <QLineEdit>
#include <QKeyEvent>
#include <QTimer>

#include "gui/customwidgets/floatingwidget.h"


namespace Ui {
class RenameOverlay;
}

class RenameOverlay : public FloatingWidget
{
    Q_OBJECT

public:
    explicit RenameOverlay(OverlayContainerWidget *parent);
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

#endif // RENAMEOVERLAY_H
