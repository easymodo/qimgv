#include "renameoverlay.h"
#include "ui_renameoverlay.h"

RenameOverlay::RenameOverlay(FloatingWidgetContainer *parent) :
    OverlayWidget(parent),
    ui(new Ui::RenameOverlay)
{
    ui->setupUi(this);
    connect(ui->cancelButton, &QPushButton::clicked, this, &RenameOverlay::onCancel);
    connect(ui->closeButton,  &IconButton::clicked, this, &RenameOverlay::hide);
    connect(ui->okButton,     &QPushButton::clicked, this, &RenameOverlay::rename);
    ui->okButton->setHighlighted(true);
    auto icontheme = settings->theme().iconTheme;
    ui->closeButton->setIconPath(":res/icons/" + icontheme + "/overlay/close-dim16.png");
    ui->headerIcon->setIconPath(":res/icons/" + icontheme + "/overlay/edit16.png");
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
    OverlayWidget::show();
    QTimer::singleShot(0, ui->fileName, SLOT(setFocus()));
}

void RenameOverlay::hide() {
    OverlayWidget::hide();
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
    } else if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        event->accept();
        rename();
    } else {
        event->ignore();
    }
}


