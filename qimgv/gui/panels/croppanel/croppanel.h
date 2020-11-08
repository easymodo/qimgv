#pragma once

#include <QWidget>
#include <QScreen>
#include <QStyleOption>
#include <QStyledItemDelegate>
#include <QAbstractItemView>
#include <QPainter>
#include <QtGlobal>
#include "gui/customwidgets/sidepanelwidget.h"
#include "gui/customwidgets/iconwidget.h"
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
    void cropAndSave(QRect);
    void cancel();
    void cropClicked();
    void selectionChanged(QRect);
    void selectAll();
    void aspectRatioChanged(QPointF);

protected:
    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);

private slots:
    void doCrop();
    void doCropSave();
    void onSelectionChange();
    void onAspectRatioChange(); // via manual input
    void onAspectRatioSelected(); // via ComboBox
    void setFocusCropBtn();
    void setFocusCropSaveBtn();

    void doCropDefaultAction();
private:
    Ui::CropPanel *ui;
    QRect cropRect;
    CropOverlay *overlay;
    QSize realSize;
};
