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
    folderSource = settings->savedPathsDiskMode() ? SOURCE_DISK : SOURCE_CONFIGURED;

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
        // deleteLater(): this may be called from inside one of these
        // widgets' own click handler (e.g. via toggleFolderSource()),
        // so deleting immediately would destroy a widget still on the stack
        tmp->deleteLater();
    }
    pathWidgets.clear();
    if(sourceToggleWidget) {
        ui->pathSelectorsLayout->removeWidget(sourceToggleWidget);
        sourceToggleWidget->deleteLater();
        sourceToggleWidget = nullptr;
    }
}

void CopyOverlay::createPathWidgets() {
    removePathWidgets();

    // key "0": switches the list below between configured folders and
    // subfolders of the current image's directory
    sourceToggleWidget = new ActionMenuItem(this);
    sourceToggleWidget->setIconPath(":/res/icons/common/menuitem/folderview16.png");
    sourceToggleWidget->setShortcutText("0");
    sourceToggleWidget->setText(folderSource == SOURCE_CONFIGURED ? tr("Disk folders") : tr("Configured folders"));
    connect(sourceToggleWidget, &ActionMenuItem::activated, this, &CopyOverlay::toggleFolderSource);
    ui->pathSelectorsLayout->addWidget(sourceToggleWidget);
    // a widget added to an already-visible layout is not shown automatically,
    // and an unshown widget is treated as empty (zero size) by the layout;
    // only force this while already visible - explicitly hiding it here
    // (i.e. while constructing, before the overlay is ever shown) would
    // stick, since a parent's later show() does not override an explicit hide
    if(!isHidden())
        sourceToggleWidget->show();

    const QStringList &activePaths = (folderSource == SOURCE_CONFIGURED) ? paths : diskPaths;
    int count = (activePaths.length() > maxPathCount) ? maxPathCount : activePaths.length();
    for(int i = 0; i < count; i++) {
        PathSelectorMenuItem *item = new PathSelectorMenuItem(this);
        item->setDirectory(activePaths.at(i));
        item->setShortcutText(shortcuts.key(i));
        connect(item, &PathSelectorMenuItem::directorySelected, this, &CopyOverlay::requestFileOperation);
        pathWidgets.append(item);
        ui->pathSelectorsLayout->addWidget(item);
        if(!isHidden())
            item->show();
    }

    // row count may have changed (toggling source, or the current
    // directory's subfolder count); force the cached sizeHint to be
    // recomputed, then resize & reposition accordingly
    ui->pathSelectorsLayout->invalidate();
    if(layout())
        layout()->invalidate();
    recalculateGeometry();
}

void CopyOverlay::createShortcuts() {
    for(int i = 0; i < maxPathCount; i++)
        shortcuts.insert(QString::number(i + 1), i);
}

void CopyOverlay::toggleFolderSource() {
    if(folderSource == SOURCE_CONFIGURED) {
        // capture any in-place edits (folder icon -> pick new directory)
        // made to the configured list before we replace the widgets
        syncConfiguredPaths();
        folderSource = SOURCE_DISK;
        refreshDiskPaths();
    } else {
        folderSource = SOURCE_CONFIGURED;
    }
    settings->setSavedPathsDiskMode(folderSource == SOURCE_DISK);
    createPathWidgets();
}

void CopyOverlay::syncConfiguredPaths() {
    QStringList updated;
    for(int i = 0; i < pathWidgets.count(); i++)
        updated << pathWidgets.at(i)->directory();
    paths = updated;
}

void CopyOverlay::refreshDiskPaths() {
    diskPaths.clear();
    if(currentDir.isEmpty())
        return;
    QDir dir(currentDir);
    const auto entries = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for(const QFileInfo &fi : entries) {
        if(diskPaths.count() >= maxPathCount)
            break;
        diskPaths << fi.absoluteFilePath();
    }
}

void CopyOverlay::setCurrentDirectory(QString dir) {
    currentDir = dir;
    if(folderSource == SOURCE_DISK) {
        refreshDiskPaths();
        createPathWidgets();
    }
}

void CopyOverlay::requestFileOperation(QString path) {
    if(mode == OVERLAY_COPY)
        emit copyRequested(path);
    else
        emit moveRequested(path);
}

void CopyOverlay::readSettings() {
    // don't interfere with the main panel
    if(settings->panelEnabled() && settings->panelPosition() == PanelPosition::PANEL_BOTTOM) {
        setPosition(FloatingWidgetPosition::TOPLEFT);
    } else {
        setPosition(FloatingWidgetPosition::BOTTOMLEFT);
    }
    update();
}

// for some reason, duplicate folders may appear in the configuration
// we remove duplicate directories
void CopyOverlay::saveSettings() {
    // the disk-listed folders are transient and not part of the user's
    // configuration; any edits made while in configured mode were already
    // captured into `paths` by syncConfiguredPaths() when switching away
    if(folderSource != SOURCE_CONFIGURED)
        return;
    paths.clear();
    QStringList temp;
    for(int i = 0; i< pathWidgets.count(); i++) {
        QString path = pathWidgets.at(i)->path();
        if (!path.isEmpty()) {
            if (!temp.contains(path)) {
                temp << path;
                paths << pathWidgets.at(i)->directory();
            }
        }
    }
    settings->setSavedPaths(paths);
}

void CopyOverlay::createDefaultPaths() {
    QString home = QDir::homePath();
    if (paths.count() < 1 || paths.at(0).isEmpty() || paths.at(0)[0] == '@') {
        paths.clear();
        paths << home;
    }
    if (paths.count() == 1 && paths.at(0) == home) {
        QDir dir(home);
        foreach(QFileInfo mfi, dir.entryInfoList()) {
            if (paths.count() >= maxPathCount) {
                break;
            }
            if(mfi.isFile()) {
                continue;
            } 
            else {
                if(mfi.fileName() == "."  
                || mfi.fileName() ==  ".."
                // hide directory
                || mfi.fileName()[0] ==  '.' 
                // windows system directory
                || mfi.fileName() ==  "3D Objects"
                || mfi.fileName() ==  "Contacts"
                || mfi.fileName() ==  "Favorites"
                || mfi.fileName() ==  "Links"
                || mfi.fileName() ==  "Saved Games"
                || mfi.fileName() ==  "Searches"
                ) {
                    continue;
                }
                QString qpath(home + "/" + mfi.fileName());
                QFileInfo qinfo(qpath);
                if (qinfo.permission(QFile::WriteUser | QFile::ReadGroup)) {
                    paths << qpath;
                }
            }
        }
    }
}

// block native tab-switching so we can use it in shortcuts
bool CopyOverlay::focusNextPrevChild(bool mode) {
    return false;
}

void CopyOverlay::keyPressEvent(QKeyEvent *event) {
    event->accept();
    QString key = actionManager->keyForNativeScancode(event->nativeScanCode());
    if(key == "0")
        toggleFolderSource();
    else if(shortcuts.contains(key) && shortcuts[key] < pathWidgets.count())
        requestFileOperation(pathWidgets.at(shortcuts[key])->directory());
    else
        actionManager->processEvent(event);
}
