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
    if(shortcuts.contains(key))
        requestFileOperation(pathWidgets.at(shortcuts[key])->directory());
    else
        actionManager->processEvent(event);
}
