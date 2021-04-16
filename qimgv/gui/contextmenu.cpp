#include "contextmenu.h"
#include "ui_contextmenu.h"

ContextMenu::ContextMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContextMenu)
{
    ui->setupUi(this);
#ifdef _WIN32
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
#else
    setWindowFlags(Qt::Popup);
#endif
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoMousePropagation, true);
    this->hide();

    // -------------------------------------------------------------------------
    // setup actions
    // top zoom buttons
    ui->zoomIn->setAction("zoomIn");
    ui->zoomIn->setIconPath(":/res/icons/common/buttons/contextmenu/zoom-in18.png");
    ui->zoomIn->setTriggerMode(TriggerMode::PressTrigger);
    ui->zoomOut->setAction("zoomOut");
    ui->zoomOut->setIconPath(":/res/icons/common/buttons/contextmenu/zoom-out18.png");
    ui->zoomOut->setTriggerMode(TriggerMode::PressTrigger);
    ui->zoomOriginal->setAction("fitNormal");
    ui->zoomOriginal->setIconPath(":/res/icons/common/buttons/contextmenu/zoom-original18.png");
    ui->zoomOriginal->setTriggerMode(TriggerMode::PressTrigger);
    ui->fitWidth->setAction("fitWidth");
    ui->fitWidth->setIconPath(":/res/icons/common/buttons/contextmenu/fit-width18.png");
    ui->fitWidth->setTriggerMode(TriggerMode::PressTrigger);
    ui->fitWindow->setAction("fitWindow");
    ui->fitWindow->setIconPath(":/res/icons/common/buttons/contextmenu/fit-window18.png");
    ui->fitWindow->setTriggerMode(TriggerMode::PressTrigger);
    // -------------------------------------------------------------------------
    //  entries
    ui->rotateLeft->setAction("rotateLeft");
    ui->rotateLeft->setText(tr("Rotate left"));
    ui->rotateLeft->setIconPath(":/res/icons/common/menuitem/rotate-left16.png");

    ui->rotateRight->setAction("rotateRight");
    ui->rotateRight->setText(tr("Rotate right"));
    ui->rotateRight->setIconPath(":/res/icons/common/menuitem/rotate-right16.png");

    ui->flipH->setAction("flipH");
    ui->flipH->setText(tr("Flip H"));
    ui->flipH->setIconPath(":/res/icons/common/menuitem/flip-h16.png");

    ui->flipV->setAction("flipV");
    ui->flipV->setText(tr("Flip V"));
    ui->flipV->setIconPath(":/res/icons/common/menuitem/flip-v16.png");

    ui->crop->setAction("crop");
    ui->crop->setText(tr("Crop"));
    ui->crop->setIconPath(":/res/icons/common/menuitem/image-crop16.png");

    ui->resize->setAction("resize");
    ui->resize->setText(tr("Resize"));
    ui->resize->setIconPath(":/res/icons/common/menuitem/resize16.png");
    // -------------------------------------------------------------------------
    ui->copy->setAction("copyFile");
    ui->copy->setText(tr("Quick copy"));
    ui->copy->setIconPath(":/res/icons/common/menuitem/copy16.png");

    ui->move->setAction("moveFile");
    ui->move->setText(tr("Quick move"));
    ui->move->setIconPath(":/res/icons/common/menuitem/move16.png");

    ui->trash->setAction("moveToTrash");
    ui->trash->setText(tr("Move to trash"));
    ui->trash->setIconPath(":/res/icons/common/menuitem/trash-red16.png");
    // -------------------------------------------------------------------------
    ui->open->setAction("open");
    ui->open->setText(tr("Open"));
    ui->open->setIconPath(":/res/icons/common/menuitem/open16.png");

    ui->folderView->setAction("folderView");
    ui->folderView->setText(tr("Folder View"));
    ui->folderView->setIconPath(":/res/icons/common/menuitem/folderview16.png");

    ui->settings->setAction("openSettings");
    ui->settings->setText(tr("Settings"));
    ui->settings->setIconPath(":/res/icons/common/menuitem/settings16.png");
    // -------------------------------------------------------------------------
    ui->showLocation->setAction("showInDirectory");
    ui->showLocation->setText(tr("Show in folder"));
    ui->showLocation->setIconPath(":/res/icons/common/menuitem/folder16.png");
    // -------------------------------------------------------------------------
    // force resize to fit new menuitem width
    this->adjustSize();
    // -------------------------------------------------------------------------
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
    ui->showLocation->setEnabled(mode);
}

void ContextMenu::showAt(QPoint pos) {
    QRect geom = geometry();
    geom.moveTopLeft(pos);
    setGeometry(geom);
    show();
}

void ContextMenu::setGeometry(QRect geom) {
    auto screen = QGuiApplication::screenAt(cursor().pos());
    // fit inside the current screen
    if(geom.bottom() > screen->geometry().bottom())
        geom.moveBottom(cursor().pos().y());
    if(geom.right() > screen->geometry().right())
        geom.moveRight(screen->geometry().right());
    QWidget::setGeometry(geom);
}

void ContextMenu::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
    hide();
}

void ContextMenu::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ContextMenu::keyPressEvent(QKeyEvent *event) {
    quint32 nativeScanCode = event->nativeScanCode();
    QString key = actionManager->keyForNativeScancode(nativeScanCode);
    // todo: keyboard navigation
    if(key == "Up") {}
    if(key == "Down") {}
    if(key == "Esc")
        hide();
    if(key == "Enter") {}
    else
        actionManager->processEvent(event);
}
