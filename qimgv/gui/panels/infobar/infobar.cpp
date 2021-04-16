#include "infobar.h"
#include "ui_infobar.h"

InfoBar::InfoBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoBar)
{
    ui->setupUi(this);
    ui->path->setText("No file opened.");
}

InfoBar::~InfoBar() {
    delete ui;
}

void InfoBar::setInfo(QString position, QString fileName, QString info) {
    ui->index->setText(position);
    ui->path->setText(fileName);
    ui->info->setText(info);
}

void InfoBar::wheelEvent(QWheelEvent *event) {
    event->accept();
}

void InfoBar::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
