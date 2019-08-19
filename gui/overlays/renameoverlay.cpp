#include "renameoverlay.h"
#include "ui_renameoverlay.h"

RenameOverlay::RenameOverlay(OverlayContainerWidget *parent) :
    FloatingWidget(parent),
    ui(new Ui::RenameOverlay)
{
    ui->setupUi(this);
    connect(ui->okButton, &QPushButton::clicked, this, &RenameOverlay::rename);
    connect(ui->cancelButton, &QPushButton::clicked, this, &RenameOverlay::onCancel);
    connect(ui->closeButton, &QPushButton::clicked, this, &RenameOverlay::hide);

    setPosition(FloatingWidgetPosition::CENTER);
    hide();
}

RenameOverlay::~RenameOverlay() {
    delete ui;
}

void RenameOverlay::show() {
    FloatingWidget::show();
    QTimer::singleShot(0, ui->fileName, SLOT(setFocus()));
}

void RenameOverlay::hide() {
    FloatingWidget::hide();
    ui->fileName->clearFocus();
}

void RenameOverlay::setName(QString name) {
    ui->fileName->setText(name);
    origName = name;
}

void RenameOverlay::rename() {
    if(ui->fileName->text().isEmpty())
        return;
    hide();
    emit renameRequested(ui->fileName->text());
}

void RenameOverlay::onCancel() {
    hide();
    ui->fileName->setText(origName);
}

void RenameOverlay::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Escape) {
        event->accept();
        onCancel();
    } else if(event->key() == Qt::Key_Return) {
        event->accept();
        rename();
    } else {
        event->ignore();
    }
}


