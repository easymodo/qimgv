#include "infobar.h"
#include "ui_infobar.h"

#include <QPainter>
#include <QStyleOption>

InfoBar::InfoBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoBar)
{
    ui->setupUi(this);
}

InfoBar::~InfoBar()
{
    delete ui;
}

void InfoBar::setInfo(int fileIndex, int fileCount, QString fileName, QSize imageSize, int fileSize) {
    ui->index->setText(QString::number(fileIndex + 1) + "/" + QString::number(fileCount));
    ui->path->setText(fileName);
    ui->resolution->setText(QString::number(imageSize.width()) + " x " + QString::number(imageSize.height()));
    ui->size->setText(QString::number(fileSize / 1024) + " KB");
}

void InfoBar::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
