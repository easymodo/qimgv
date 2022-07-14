#include "sidepanel.h"
#include "ui_sidepanel.h"

SidePanel::SidePanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SidePanel),
    mWidget(nullptr)
{
    ui->setupUi(this);
    this->setObjectName("SidePanel");
    this->hide();
}

SidePanel::~SidePanel() {
    delete ui;
}

void SidePanel::setWidget(SidePanelWidget* w) {
    if(mWidget) {
        mWidget->hide();
        ui->layout->removeWidget(mWidget);
    }
    mWidget = w;
    ui->layout->addWidget(w);
    w->show();
}

SidePanelWidget* SidePanel::widget() {
    return mWidget;
}

void SidePanel::show() {
    QWidget::show();
    if(mWidget)
        mWidget->show();
}

void SidePanel::hide() {
    if(mWidget) {
        mWidget->hide();
    }
    QWidget::hide();
}

void SidePanel::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
