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
    ui->zoomOut->setAction("zoomOut");
    ui->zoomOriginal->setAction("fitNormal");
    ui->fitWidth->setAction("fitWidth");
    ui->fitWindow->setAction("fitWindow");
    // -------------------------------------------------------------------------
    //  entries
    ui->rotateLeft->setAction("rotateLeft");
    ui->rotateLeft->setText("Rotate left");
    ui->rotateLeft->setPixmap(QPixmap(":/res/icons/buttons/rotate-left16.png"));

    ui->rotateRight->setAction("rotateRight");
    ui->rotateRight->setText("Rotate right");
    ui->rotateRight->setPixmap(QPixmap(":/res/icons/buttons/rotate-right16.png"));

    ui->flipH->setAction("flipH");
    ui->flipH->setText("Flip H");
    ui->flipH->setPixmap(QPixmap(":/res/icons/buttons/flip-h16.png"));

    ui->flipV->setAction("flipV");
    ui->flipV->setText("Flip V");
    ui->flipV->setPixmap(QPixmap(":/res/icons/buttons/flip-v16.png"));

    ui->crop->setAction("crop");
    ui->crop->setText("Crop");
    ui->crop->setPixmap(QPixmap(":/res/icons/buttons/image-crop16.png"));

    ui->resize->setAction("resize");
    ui->resize->setText("Resize");
    ui->resize->setPixmap(QPixmap(":/res/icons/buttons/view-fullscreen16.png"));
    // -------------------------------------------------------------------------
    ui->copy->setAction("copyFile");
    ui->copy->setText("Quick copy");
    ui->copy->setPixmap(QPixmap(":/res/icons/buttons/copy16.png"));

    ui->move->setAction("moveFile");
    ui->move->setText("Quick move");
    ui->move->setPixmap(QPixmap(":/res/icons/buttons/move16.png"));

    ui->trash->setAction("moveToTrash");
    ui->trash->setText("Move to trash");
    ui->trash->setPixmap(QPixmap(":/res/icons/buttons/trash-red16.png"));
    // -------------------------------------------------------------------------
    ui->open->setAction("open");
    ui->open->setText("Open");
    ui->open->setPixmap(QPixmap(":/res/icons/buttons/open16.png"));

    ui->folderView->setAction("folderView");
    ui->folderView->setText("Folder View");
    ui->folderView->setPixmap(QPixmap(":/res/icons/buttons/folderview16v2.png"));

    ui->settings->setAction("openSettings");
    ui->settings->setText("Settings");
    ui->settings->setPixmap(QPixmap(":/res/icons/buttons/settings16.png"));
    // -------------------------------------------------------------------------
    connect(this, SIGNAL(pressed()), this, SLOT(onPressed()));
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

//hide self on click
void ContextMenu::onPressed() {
    this->hide();
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
