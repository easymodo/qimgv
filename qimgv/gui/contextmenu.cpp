#include "contextmenu.h"
#include "ui_contextmenu.h"

ContextMenu::ContextMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContextMenu)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup);
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
    // transform buttons
    ui->rotateLeft->setAction("rotateLeft");
    ui->rotateLeft->setIconPath(":/res/icons/common/menuitem/rotate-left16.png");
    ui->rotateLeft->setTriggerMode(TriggerMode::PressTrigger);
    ui->rotateRight->setAction("rotateRight");
    ui->rotateRight->setIconPath(":/res/icons/common/menuitem/rotate-right16.png");
    ui->rotateRight->setTriggerMode(TriggerMode::PressTrigger);
    ui->flipH->setAction("flipH");
    ui->flipH->setIconPath(":/res/icons/common/menuitem/flip-h16.png");
    ui->flipH->setTriggerMode(TriggerMode::PressTrigger);
    ui->flipV->setAction("flipV");
    ui->flipV->setIconPath(":/res/icons/common/menuitem/flip-v16.png");
    ui->flipV->setTriggerMode(TriggerMode::PressTrigger);
    ui->crop->setAction("crop");
    ui->crop->setIconPath(":/res/icons/common/menuitem/image-crop16.png");
    ui->crop->setTriggerMode(TriggerMode::PressTrigger);
    ui->resize->setAction("resize");
    ui->resize->setIconPath(":/res/icons/common/menuitem/resize16.png");
    ui->resize->setTriggerMode(TriggerMode::PressTrigger);
    // -------------------------------------------------------------------------
    //  entries
    ui->print->setAction("print");
    ui->print->setText(tr("Print"));
    ui->print->setIconPath(":/res/icons/common/menuitem/print16.png");

    ui->copy->setAction("copyFile");
    ui->copy->setText(tr("Quick copy"));
    ui->copy->setIconPath(":/res/icons/common/menuitem/copy16.png");

    ui->move->setAction("moveFile");
    ui->move->setText(tr("Quick move"));
    ui->move->setIconPath(":/res/icons/common/menuitem/move16.png");

    ui->trash->setAction("moveToTrash");
    ui->trash->setText(tr("Move to trash"));
    ui->trash->setIconPath(":/res/icons/common/menuitem/trash16.png");
    ui->trash->setShortcutText("");
    // -------------------------------------------------------------------------
    ui->open->setAction("open");
    ui->open->setText(tr("Open"));
    ui->open->setIconPath(":/res/icons/common/menuitem/open16.png");
    ui->open->setShortcutText("");

    ui->folderView->setAction("folderView");
    ui->folderView->setText(tr("Folder View"));
    ui->folderView->setIconPath(":/res/icons/common/menuitem/folderview16.png");
    ui->folderView->setShortcutText("");

    ui->settings->setAction("openSettings");
    ui->settings->setText(tr("Settings"));
    ui->settings->setIconPath(":/res/icons/common/menuitem/settings16.png");
    // -------------------------------------------------------------------------
    ui->openWith->setText(tr("Open with..."));
    ui->openWith->setIconPath(":/res/icons/common/menuitem/run16.png");
    ui->openWith->setPassthroughClicks(false);
    connect(ui->openWith, &ContextMenuItem::pressed, this, &ContextMenu::switchToScriptsPage);
    // -------------------------------------------------------------------------
    ui->showLocation->setAction("showInDirectory");
    ui->showLocation->setText(tr("Show in folder"));
    ui->showLocation->setIconPath(":/res/icons/common/menuitem/folder16.png");
    // -------------------------------------------------------------------------
    // force resize to fit new menuitem width
    this->adjustSize();

    // Scripts page
    // -------------------------------------------------------------------------
    ui->backButton->setText(tr("Back"));
    ui->backButton->setIconPath(":/res/icons/common/menuitem/back16.png");
    ui->backButton->setPassthroughClicks(false);
    ui->scriptSetupButton->setText(tr("Configure menu"));
    ui->scriptSetupButton->setIconPath(":/res/icons/common/menuitem/settings16.png");
    connect(ui->backButton, &ContextMenuItem::pressed, this, &ContextMenu::switchToMainPage);
    connect(ui->scriptSetupButton, &ContextMenuItem::pressed, this, &ContextMenu::showScriptSettings);
    fillOpenWithMenu();
}

ContextMenu::~ContextMenu() {
    delete ui;
}

void ContextMenu::fillOpenWithMenu() {
    auto scripts = scriptManager->allScripts();
    QMap<QString, Script>::iterator i;
    for (i = scripts.begin(); i != scripts.end(); ++i) {
        if(!i.value().command.isEmpty()) {
            auto btn = new ContextMenuItem();
            btn->setAction("s:"+i.key());
            btn->setIconPath(":/res/icons/common/menuitem/open16.png");
            btn->setText(i.key());
            ui->scriptsLayout->addWidget(btn);
        }
    }
}

void ContextMenu::switchToMainPage() {
    ui->stackedWidget->setCurrentIndex(0);
}

void ContextMenu::switchToScriptsPage() {
    ui->stackedWidget->setCurrentIndex(1);
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
    ui->openWith->setEnabled(mode);
    ui->showLocation->setEnabled(mode);
}

void ContextMenu::showAt(QPoint pos) {
    switchToMainPage();
    QRect geom = geometry();
    geom.moveTopLeft(pos);
    setGeometry(geom);
    show();
}

void ContextMenu::setGeometry(QRect geom) {
    auto screen = QGuiApplication::screenAt(cursor().pos());
    if(!screen) // fallback
        screen = QGuiApplication::primaryScreen();
    if(screen) {
        // fit inside the current screen
        if(geom.bottom() > screen->geometry().bottom())
            geom.moveBottom(cursor().pos().y());
        if(geom.right() > screen->geometry().right())
            geom.moveRight(screen->geometry().right());
    }
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
