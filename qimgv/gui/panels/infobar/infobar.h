#pragma once

#include <QWidget>
#include <QWheelEvent>
#include <QDebug>

#include <QPainter>
#include <QStyleOption>

namespace Ui {
class InfoBar;
}

class InfoBar : public QWidget
{
    Q_OBJECT

public:
    explicit InfoBar(QWidget *parent = nullptr);
    ~InfoBar();

public slots:
    void setInfo(QString position, QString fileName, QString info);
protected:
    void paintEvent(QPaintEvent *event);
    void wheelEvent(QWheelEvent *event);
private:
    Ui::InfoBar *ui;
};
