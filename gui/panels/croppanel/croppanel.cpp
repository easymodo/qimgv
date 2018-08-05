#include "croppanel.h"
#include "ui_croppanel.h"

CropPanel::CropPanel(CropOverlay *_overlay, QWidget *parent) :
    overlay(_overlay),
    SidePanelWidget(parent),
    ui(new Ui::CropPanel)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::NoFocus);
    hide();
    connect(ui->cancelButton, SIGNAL(clicked()), this, SIGNAL(cancel()));
    connect(ui->cropButton, SIGNAL(clicked()), this, SLOT(onCropClicked()));
    connect(ui->width, SIGNAL(valueChanged(int)), this, SLOT(onSelectionChange()));
    connect(ui->height, SIGNAL(valueChanged(int)), this, SLOT(onSelectionChange()));
    connect(ui->posX, SIGNAL(valueChanged(int)), this, SLOT(onSelectionChange()));
    connect(ui->posY, SIGNAL(valueChanged(int)), this, SLOT(onSelectionChange()));

    connect(overlay, SIGNAL(selectionChanged(QRect)),
            this, SLOT(onSelectionOutsideChange(QRect)));
    connect(this, SIGNAL(selectionChanged(QRect)),
            overlay, SLOT(onSelectionOutsideChange(QRect)));
    connect(overlay, SIGNAL(escPressed()), this, SIGNAL(cancel()));
    connect(overlay, SIGNAL(enterPressed()), this, SLOT(onCropClicked()));
    connect(this, SIGNAL(selectAll()), overlay, SLOT(selectAll()));
}

CropPanel::~CropPanel()
{
    delete ui;
}

void CropPanel::setImageRealSize(QSize sz) {
    ui->width->setMaximum(sz.width());
    ui->height->setMaximum(sz.height());
    realSize = sz;
}

void CropPanel::onCropClicked() {
    QRect target(ui->posX->value(), ui->posY->value(),
                 ui->width->value(), ui->height->value());
    if(target.width() > 0 && target.height() > 0 && target.size() != realSize)
        emit crop(target);
    else
        emit cancel();
}

// on user input
void CropPanel::onSelectionChange() {
    emit selectionChanged(QRect(ui->posX->value(),
                                ui->posY->value(),
                                ui->width->value(),
                                ui->height->value()));
}

// update input box values
void CropPanel::onSelectionOutsideChange(QRect rect) {
    ui->width->blockSignals(true);
    ui->height->blockSignals(true);
    ui->posX->blockSignals(true);
    ui->posY->blockSignals(true);

    ui->width->setValue(rect.width());
    ui->height->setValue(rect.height());
    ui->posX->setValue(rect.left());
    ui->posY->setValue(rect.top());

    ui->width->blockSignals(false);
    ui->height->blockSignals(false);
    ui->posX->blockSignals(false);
    ui->posY->blockSignals(false);
}

void CropPanel::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CropPanel::show() {
    QWidget::show();
    // stackoverflow sorcery
    QTimer::singleShot(0,ui->width,SLOT(setFocus()));
}

void CropPanel::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        emit onCropClicked();
    } else if(event->key() == Qt::Key_Escape) {
        emit cancel();
    } else if(event->matches(QKeySequence::SelectAll)) {
        emit selectAll();
    } else {
        event->ignore();
    }
}
