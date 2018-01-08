#ifndef CROPPANEL_H
#define CROPPANEL_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>

namespace Ui {
class CropPanel;
}

class CropPanel : public QWidget
{
    Q_OBJECT

public:
    explicit CropPanel(QWidget *parent = 0);
    ~CropPanel();

signals:
    void crop(QRect);
    void cancel();

protected:
    void paintEvent(QPaintEvent *);

private slots:
    void onCropPressed();
private:
    Ui::CropPanel *ui;
    QRect cropRect;
};

#endif // CROPPANEL_H
