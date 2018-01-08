#ifndef CROPPANEL_H
#define CROPPANEL_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include <QSpinBox>
#include <QDebug>

namespace Ui {
class CropPanel;
}

class CropPanel : public QWidget
{
    Q_OBJECT

public:
    explicit CropPanel(QWidget *parent = 0);
    ~CropPanel();

public slots:
    void onSelectionOutsideChange(QRect rect);
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
