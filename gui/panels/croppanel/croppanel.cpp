#include "croppanel.h"
#include "ui_croppanel.h"

CropPanel::CropPanel(CropOverlay *_overlay, QWidget *parent) :
    SidePanelWidget(parent),
    ui(new Ui::CropPanel),
    overlay(_overlay)
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
    connect(ui->ARX, SIGNAL(valueChanged(double)), this, SLOT(onAspectRatioChange()));
    connect(ui->ARY, SIGNAL(valueChanged(double)), this, SLOT(onAspectRatioChange()));
    connect(ui->ARcomboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onAspectRatioSelected()));

    connect(overlay, SIGNAL(selectionChanged(QRect)),
            this, SLOT(onSelectionOutsideChange(QRect)));
    connect(this, SIGNAL(selectionChanged(QRect)),
            overlay, SLOT(onSelectionOutsideChange(QRect)));
    connect(this, SIGNAL(aspectRatioChanged(QPointF)),
            overlay, SLOT(setAspectRatio(QPointF)));
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
    // reset to free mode on image change
    ui->ARcomboBox->setCurrentIndex(0);
    // update aspect ratio in input fields

    onAspectRatioSelected();
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

void CropPanel::onAspectRatioChange() {
    ui->ARcomboBox->setCurrentIndex(1); // "Custom"
    if(ui->ARX->value() && ui->ARY->value())
        emit aspectRatioChanged(QPointF(ui->ARX->value(), ui->ARY->value()));
}

// 0 == free
// 1 == custom (from input fields)
// 2 == current image
// 3 == current screen
// 4 ...
void CropPanel::onAspectRatioSelected() {
    QPointF newAR(1, 1);

    int index = ui->ARcomboBox->currentIndex();
    switch(index) {
    case 0:
    {
        overlay->setForceAspectRatio(false);
        if(realSize.height() != 0)
            newAR = QPointF(qreal(realSize.width()) / realSize.height(), 1.0);
        break;
    }
    case 1:
    {
        newAR = QPointF(ui->ARX->value(), ui->ARY->value());
        break;
    }
    case 2:
    {
        newAR = QPointF(qreal(realSize.width()) / realSize.height(), 1.0);
        break;
    }
    case 3:
    {
        QScreen* screen = QGuiApplication::screenAt(mapToGlobal(ui->ARcomboBox->geometry().topLeft()));
        if(screen == nullptr)
            screen = QGuiApplication::primaryScreen();
        newAR = QPointF(qreal(screen->geometry().width()) / screen->geometry().height(), 1.0);
        break;
    }
    case 4:
    {
        newAR = QPointF(1.0, 1.0);
        break;
    }
    case 5:
    {
        newAR = QPointF(4.0, 3.0);
        break;
    }
    case 6:
    {
        newAR = QPointF(16.0, 9.0);
        break;
    }
    case 7:
    {
        newAR = QPointF(16.0, 10.0);
        break;
    }
    default: // apply aspect ratio; update input fields
    {
        break;
    }
    }

    ui->ARX->blockSignals(true);
    ui->ARY->blockSignals(true);
    ui->ARX->setValue(newAR.x());
    ui->ARY->setValue(newAR.y());
    ui->ARX->blockSignals(false);
    ui->ARY->blockSignals(false);
    if(index)
        overlay->setAspectRatio(newAR);
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
