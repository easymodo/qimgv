#include "renameoverlay.h"
#include "ui_renameoverlay.h"

RenameOverlay::RenameOverlay(OverlayContainerWidget *parent) :
    FloatingWidget(parent),
    ui(new Ui::RenameOverlay)
{
    ui->setupUi(this);
    connect(ui->cancelButton, &QPushButton::clicked, this, &RenameOverlay::onCancel);
    connect(ui->closeButton,  &IconButton::clicked, this, &RenameOverlay::hide);
    connect(ui->okButton,     &QPushButton::clicked, this, &RenameOverlay::rename);
    ui->closeButton->setIconPath(":res/icons/buttons/close-dim16.png");
    ui->headerIcon->setIconPath(":res/icons/buttons/edit16.png");
    setPosition(FloatingWidgetPosition::CENTER);
    setAcceptKeyboardFocus(true);
    hide();
    if(parent)
        setContainerSize(parent->size());
}

RenameOverlay::~RenameOverlay() {
    delete ui;
}

void RenameOverlay::show() {
    selectName();
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
    selectName();
}

void RenameOverlay::selectName() {
    int end = ui->fileName->text().lastIndexOf(".");
    ui->fileName->setSelection(0, end);
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


