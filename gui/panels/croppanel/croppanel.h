#ifndef CROPPANEL_H
#define CROPPANEL_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include "gui/customwidgets/sidepanelwidget.h"
#include "gui/customwidgets/spinboxinputfix.h"
#include <QTimer>
#include <QDebug>

namespace Ui {
class CropPanel;
}

class CropPanel : public SidePanelWidget
{
    Q_OBJECT

public:
    explicit CropPanel(QWidget *parent = 0);
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

protected:
    void paintEvent(QPaintEvent *);

private slots:
    void onCropPressed();
    void onSelectionChange();

private:
    Ui::CropPanel *ui;
    QRect cropRect;
};

#endif // CROPPANEL_H
