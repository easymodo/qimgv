#include "fvoptionspopup.h"
#include "ui_fvoptionspopup.h"

FVOptionsPopup::FVOptionsPopup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FVOptionsPopup)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground, true);

    ui->viewSimpleButton->setText(QObject::tr("Simple"));
    ui->viewExtendedButton->setText(QObject::tr("Extended"));
    ui->viewFoldersButton->setText(QObject::tr("Extended + Folders"));

    connect(ui->viewSimpleButton,   &ContextMenuItem::pressed, this, &FVOptionsPopup::selectSimpleView);
    connect(ui->viewExtendedButton, &ContextMenuItem::pressed, this, &FVOptionsPopup::selectExtendedView);
    connect(ui->viewFoldersButton,  &ContextMenuItem::pressed, this, &FVOptionsPopup::selectFoldersView);

    // force size recalculation
    this->adjustSize();

    readSettings();
    connect(settings, &Settings::settingsChanged, this, &FVOptionsPopup::readSettings);

    hide();
}


FVOptionsPopup::~FVOptionsPopup() {
    delete ui;
}

void FVOptionsPopup::setSimpleView() {
    ui->viewSimpleButton->setIconPath(":res/icons/common/buttons/panel-small/add-new12.png");
    ui->viewExtendedButton->setIconPath("");
    ui->viewFoldersButton->setIconPath("");
}

void FVOptionsPopup::setExtendedView() {
    ui->viewSimpleButton->setIconPath("");
    ui->viewExtendedButton->setIconPath(":res/icons/common/buttons/panel-small/add-new12.png");
    ui->viewFoldersButton->setIconPath("");
}

void FVOptionsPopup::setFoldersView() {
    ui->viewSimpleButton->setIconPath("");
    ui->viewExtendedButton->setIconPath("");
    ui->viewFoldersButton->setIconPath(":res/icons/common/buttons/panel-small/add-new12.png");
}

void FVOptionsPopup::selectSimpleView() {
    setSimpleView();
    emit viewModeSelected(FV_SIMPLE);
}

void FVOptionsPopup::selectExtendedView() {
    setExtendedView();
    emit viewModeSelected(FV_EXTENDED);
}

void FVOptionsPopup::selectFoldersView() {
    setFoldersView();
    emit viewModeSelected(FV_EXT_FOLDERS);
}

void FVOptionsPopup::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void FVOptionsPopup::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Escape)
        hide();
    else
        actionManager->processEvent(event);
}

void FVOptionsPopup::setViewMode(FolderViewMode mode) {
    if(mode == FV_SIMPLE)
        setSimpleView();
    else if(mode == FV_EXTENDED)
        setExtendedView();
    else
        setFoldersView();
}

void FVOptionsPopup::readSettings() {
    setViewMode(settings->folderViewMode());
}

void FVOptionsPopup::showAt(QPoint pos) {
    QRect geom = geometry();
    geom.moveTopLeft(pos);
    setGeometry(geom);
    show();
}

void FVOptionsPopup::hideEvent(QHideEvent* event) {
    event->accept();
    emit dismissed();
}
