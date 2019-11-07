#pragma once

#include <QWidget>
#include <QDebug>
#include <QStyleOption>
#include <QPainter>
#include <QVBoxLayout>
#include "gui/customwidgets/sidepanelwidget.h"

namespace Ui {
class SidePanel;
}

class SidePanel : public QWidget
{
    Q_OBJECT

public:
    explicit SidePanel(QWidget *parent = nullptr);
    ~SidePanel();

    void setWidget(SidePanelWidget *w);
    SidePanelWidget* widget();

public slots:
    void show();
    void hide();

protected:
    void paintEvent(QPaintEvent *);
private:
    Ui::SidePanel *ui;
    SidePanelWidget *mWidget;
};
