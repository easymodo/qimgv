#ifndef CROPPANEL_H
#define CROPPANEL_H

#include <QWidget>
#include <QScreen>
#include <QStyleOption>
#include <QPainter>
#include <QtGlobal>
#include "gui/customwidgets/sidepanelwidget.h"
#include "gui/customwidgets/spinboxinputfix.h"
#include "gui/overlays/cropoverlay.h"
#include <QTimer>
#include <QDebug>

namespace Ui {
class CropPanel;
}

class CropPanel : public SidePanelWidget
{
    Q_OBJECT

public:
    explicit CropPanel(CropOverlay *_overlay, QWidget *parent = nullptr);
    ~CropPanel();
    void setImageRealSize(QSize);

public slots:
    void onSelectionOutsideChange(QRect rect);
    void show();

signals:
    void crop(QRect);
    void cancel();
    void cropClicked();
    void selectionChanged(QRect);
    void selectAll();
    void aspectRatioChanged(QPointF);

protected:
    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void onCropClicked();
    void onSelectionChange();
    void onAspectRatioChange(); // via manual input
    void onAspectRatioSelected(); // via ComboBox

private:
    Ui::CropPanel *ui;
    QRect cropRect;
    CropOverlay *overlay;
    QSize realSize;
};

#endif // CROPPANEL_H
