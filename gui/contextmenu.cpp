#include "contextmenu.h"
#include "ui_contextmenu.h"

ContextMenu::ContextMenu(QWidget *parent) :
    ClickableWidget(parent),
    ui(new Ui::ContextMenu)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus);
    setAttribute(Qt::WA_X11NetWmWindowTypeMenu, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoMousePropagation, true);
    this->hide();

    // -------------------------------------------------------------------------
    // setup actions
    // top zoom buttons
    ui->zoomIn->setAction("zoomIn");
    ui->zoomIn->setTriggerMode(TriggerMode::PressTrigger);
    ui->zoomOut->setAction("zoomOut");
    ui->zoomOut->setTriggerMode(TriggerMode::PressTrigger);
    ui->zoomOriginal->setAction("fitNormal");
    ui->zoomOriginal->setTriggerMode(TriggerMode::PressTrigger);
    ui->fitWidth->setAction("fitWidth");
    ui->fitWidth->setTriggerMode(TriggerMode::PressTrigger);
    ui->fitWindow->setAction("fitWindow");
    ui->fitWindow->setTriggerMode(TriggerMode::PressTrigger);
    // -------------------------------------------------------------------------
    //  entries
    ui->rotateLeft->setAction("rotateLeft");
    ui->rotateLeft->setText("Rotate left");
    ui->rotateLeft->setIcon(QIcon(":/res/icons/buttons/rotate-left16.png"));

    ui->rotateRight->setAction("rotateRight");
    ui->rotateRight->setText("Rotate right");
    ui->rotateRight->setIcon(QIcon(":/res/icons/buttons/rotate-right16.png"));

    ui->flipH->setAction("flipH");
    ui->flipH->setText("Flip H");
    ui->flipH->setIcon(QIcon(":/res/icons/buttons/flip-h16.png"));

    ui->flipV->setAction("flipV");
    ui->flipV->setText("Flip V");
    ui->flipV->setIcon(QIcon(":/res/icons/buttons/flip-v16.png"));

    ui->crop->setAction("crop");
    ui->crop->setText("Crop");
    ui->crop->setIcon(QIcon(":/res/icons/buttons/image-crop16.png"));

    ui->resize->setAction("resize");
    ui->resize->setText("Resize");
    ui->resize->setIcon(QIcon(":/res/icons/buttons/view-fullscreen16.png"));
    // -------------------------------------------------------------------------
    ui->copy->setAction("copyFile");
    ui->copy->setText("Quick copy");
    ui->copy->setIcon(QIcon(":/res/icons/buttons/copy16.png"));

    ui->move->setAction("moveFile");
    ui->move->setText("Quick move");
    ui->move->setIcon(QIcon(":/res/icons/buttons/move16.png"));

    ui->trash->setAction("moveToTrash");
    ui->trash->setText("Move to trash");
    ui->trash->setIcon(QIcon(":/res/icons/buttons/trash-red16.png"));
    // -------------------------------------------------------------------------
    ui->open->setAction("open");
    ui->open->setText("Open");
    ui->open->setIcon(QIcon(":/res/icons/buttons/open16.png"));

    ui->folderView->setAction("folderView");
    ui->folderView->setText("Folder View");
    ui->folderView->setIcon(QIcon(":/res/icons/buttons/folderview16.png"));

    ui->settings->setAction("openSettings");
    ui->settings->setText("Settings");
    ui->settings->setIcon(QIcon(":/res/icons/buttons/settings16.png"));
    // -------------------------------------------------------------------------
    connect(this, SIGNAL(pressed()), this, SLOT(hide()));
}

ContextMenu::~ContextMenu() {
    delete ui;
}

void ContextMenu::setImageEntriesEnabled(bool mode) {
    ui->rotateLeft->setEnabled(mode);
    ui->rotateRight->setEnabled(mode);
    ui->flipH->setEnabled(mode);
    ui->flipV->setEnabled(mode);
    ui->crop->setEnabled(mode);
    ui->resize->setEnabled(mode);
    ui->copy->setEnabled(mode);
    ui->move->setEnabled(mode);
    ui->trash->setEnabled(mode);
}

void ContextMenu::showAt(QPoint pos) {
    QRect geom = geometry();
    geom.moveTopLeft(pos);
    setGeometry(geom);
    show();
}

void ContextMenu::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
