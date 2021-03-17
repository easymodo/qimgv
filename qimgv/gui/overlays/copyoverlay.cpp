#include "copyoverlay.h"
#include "ui_copyoverlay.h"

CopyOverlay::CopyOverlay(FloatingWidgetContainer *parent) :
    OverlayWidget(parent),
    ui(new Ui::CopyOverlay)
{
    ui->setupUi(this);
    hide();
    setFadeEnabled(true);

    ui->closeButton->setIconPath(":/res/icons/common/overlay/close-dim16.png");
    ui->headerIcon->setIconPath(":/res/icons/common/overlay/copy16.png");
    ui->headerLabel->setText(tr("Copy to..."));
    mode = OVERLAY_COPY;

    createShortcuts();

    paths = settings->savedPaths();
    if(paths.count() < maxPathCount)
        createDefaultPaths();
    createPathWidgets();

    setAcceptKeyboardFocus(true);

    if(parent)
        setContainerSize(parent->size());

    readSettings();
    connect(settings, &Settings::settingsChanged, this, &CopyOverlay::readSettings);
}

CopyOverlay::~CopyOverlay() {
    delete ui;
}

void CopyOverlay::show() {
    OverlayWidget::show();
    setFocus();
}

void CopyOverlay::hide() {
    OverlayWidget::hide();
}

void CopyOverlay::setDialogMode(CopyOverlayMode _mode) {
    mode = _mode;
    if(mode == OVERLAY_COPY) {
        ui->headerIcon->setIconPath(":/res/icons/common/overlay/copy16.png");
        ui->headerLabel->setText(tr("Copy to..."));
    } else {
        ui->headerIcon->setIconPath(":/res/icons/common/overlay/move16.png");
        ui->headerLabel->setText(tr("Move to..."));
    }
}

CopyOverlayMode CopyOverlay::operationMode() {
    return mode;
}

void CopyOverlay::removePathWidgets() {
    for(int i = 0; i < pathWidgets.count(); i++) {
        QWidget *tmp = pathWidgets.at(i);
        ui->pathSelectorsLayout->removeWidget(tmp);
        delete tmp;
    }
    pathWidgets.clear();
}

void CopyOverlay::createPathWidgets() {
    removePathWidgets();
    int count = (paths.length() > maxPathCount) ? maxPathCount : paths.length();
    for(int i = 0; i < count; i++) {
        PathSelectorMenuItem *item = new PathSelectorMenuItem(this);
        item->setDirectory(paths.at(i));
        item->setShortcutText(shortcuts.key(i));
        connect(item, &PathSelectorMenuItem::directorySelected, this, &CopyOverlay::requestFileOperation);
        pathWidgets.append(item);
        ui->pathSelectorsLayout->addWidget(item);
    }
}

void CopyOverlay::createShortcuts() {
    for(int i = 0; i < maxPathCount; i++)
        shortcuts.insert(QString::number(i + 1), i);
}

void CopyOverlay::requestFileOperation(QString path) {
    if(mode == OVERLAY_COPY)
        emit copyRequested(path);
    else
        emit moveRequested(path);
}

void CopyOverlay::readSettings() {
    // don't interfere with the main panel
    if(settings->panelEnabled() && settings->panelPosition() == PanelHPosition::PANEL_BOTTOM) {
        setPosition(FloatingWidgetPosition::TOPLEFT);
    } else {
        setPosition(FloatingWidgetPosition::BOTTOMLEFT);
    }
    update();
}

void CopyOverlay::saveSettings() {
    paths.clear();
    for(int i = 0; i< pathWidgets.count(); i++) {
        paths << pathWidgets.at(i)->directory();
    }
    settings->setSavedPaths(paths);
}

void CopyOverlay::createDefaultPaths() {
    while(paths.count() < maxPathCount) {
        paths << QDir::homePath();
    }
}

// block native tab-switching so we can use it in shortcuts
bool CopyOverlay::focusNextPrevChild(bool mode) {
    return false;
}

void CopyOverlay::keyPressEvent(QKeyEvent *event) {
    event->accept();
    QString key = actionManager->keyForNativeScancode(event->nativeScanCode());
    if(shortcuts.contains(key))
        requestFileOperation(pathWidgets.at(shortcuts[key])->directory());
    else
        actionManager->processEvent(event);
}
