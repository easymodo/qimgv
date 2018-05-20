#include "actionmanager.h"

ActionManager *actionManager = nullptr;

ActionManager::ActionManager(QObject *parent) : QObject(parent) {
}

ActionManager::~ActionManager() {
    delete actionManager;
}

ActionManager *ActionManager::getInstance() {
    if(!actionManager) {
        actionManager = new ActionManager();
        initDefaults();
        initShortcuts();
    }
    return actionManager;
}

void ActionManager::initDefaults() {
    actionManager->defaults.insert("Right", "nextImage");
    actionManager->defaults.insert("Left", "prevImage");
    actionManager->defaults.insert("XButton2", "nextImage");
    actionManager->defaults.insert("XButton1", "prevImage");
    actionManager->defaults.insert("WheelUp", "nextImage");
    actionManager->defaults.insert("WheelDown", "prevImage");
    actionManager->defaults.insert("F", "toggleFullscreen");
    actionManager->defaults.insert("F11", "toggleFullscreen");
    actionManager->defaults.insert("LMB_DoubleClick", "toggleFullscreen");
    actionManager->defaults.insert("RMB_DoubleClick", "toggleFitMode");
    actionManager->defaults.insert("MiddleButton", "exit");
    actionManager->defaults.insert("Space", "toggleFitMode");
    actionManager->defaults.insert("1", "fitWindow");
    actionManager->defaults.insert("2", "fitWidth");
    actionManager->defaults.insert("3", "fitNormal");
    actionManager->defaults.insert("R", "resize");
    actionManager->defaults.insert("H", "flipH");
    actionManager->defaults.insert("V", "flipV");
    actionManager->defaults.insert("Ctrl+R", "rotateRight");
    actionManager->defaults.insert("Ctrl+L", "rotateLeft");
    actionManager->defaults.insert("Ctrl+WheelDown", "zoomInCursor");
    actionManager->defaults.insert("Ctrl+WheelUp", "zoomOutCursor");
    actionManager->defaults.insert("Ctrl+Up", "zoomIn");
    actionManager->defaults.insert("Ctrl+Down", "zoomOut");
    actionManager->defaults.insert("Up", "scrollUp");
    actionManager->defaults.insert("Down", "scrollDown");
    actionManager->defaults.insert("Ctrl+O", "open");
    actionManager->defaults.insert("Ctrl+S", "save");
    actionManager->defaults.insert("Ctrl+Shift+S", "saveAs");
    //actionManager->defaults.insert("Ctrl+W", "setWallpaper");
    actionManager->defaults.insert("X", "crop");
    actionManager->defaults.insert("Ctrl+P", "openSettings");
    actionManager->defaults.insert("Alt+X", "exit");
    actionManager->defaults.insert("Ctrl+Q", "exit");
    actionManager->defaults.insert("escape", "closeFullScreenOrExit");
    actionManager->defaults.insert("Shift+delete", "removeFile");
    actionManager->defaults.insert("C", "copyFile");
    actionManager->defaults.insert("M", "moveFile");
    actionManager->defaults.insert("home", "jumpToFirst");
    actionManager->defaults.insert("end", "jumpToLast");
}

void ActionManager::initShortcuts() {
    actionManager->readShortcuts();
    if(actionManager->shortcuts.isEmpty()) {
        actionManager->resetDefaults();
    }
}

void ActionManager::addShortcut(QString keys, QString action) {
    if(appActions->getMap().contains(action)) {
        actionManager->shortcuts.insert(keys, action);
    } else {
        qDebug() << "ActionManager: action " << action << " is invalid.";
    }
}

void ActionManager::removeShortcut(QString keys) {
    actionManager->shortcuts.remove(keys);
}

QStringList ActionManager::actionList() {
    QStringList actionList;
    QMapIterator<QString, QVersionNumber> i(appActions->getMap());
    while(i.hasNext()) {
        i.next();
        actionList.append(i.key());
    }
    actionList.sort();
    return actionList;
}

const QStringList ActionManager::keys() {
    QStringList list;
    QMapIterator<int, QString> i(inputMap->keys());
    while(i.hasNext()) {
        i.next();
        list << i.value();
    }
    list << "XButton1" << "XButton2" << "LMB" << "RMB" << "MiddleButton" << "WheelUp" << "WheelDown";
    return list;
}

const QMap<QString, QString> &ActionManager::allShortcuts() {
    return actionManager->shortcuts;
}

void ActionManager::removeAllShortcuts() {
    shortcuts.clear();
}

QString ActionManager::keyForNativeScancode(int scanCode) {
    if(inputMap->keys().contains(scanCode)) {
        return inputMap->keys()[scanCode];
    } else {
        return "";
    }
}

void ActionManager::resetDefaults() {
    actionManager->shortcuts = actionManager->defaults;
}

// argument: target version
// every action added from next version onwards will be reset
// TODO: maybe move this to core or something?
// TODO: save this to settings immediately
// settings <=> actionManager <=> settingsDialog
void ActionManager::resetDefaultsFromVersion(QVersionNumber lastVer) {
    QMapIterator<QString, QString> i(defaults);
    while(i.hasNext()) {
        i.next();
        if(appActions->getMap().value(i.value()) > lastVer) {
            if(!shortcuts.contains(i.key())) {
                shortcuts.insert(i.key(), i.value());
                qDebug() << "ActionManager: new action " << i.value() << " - assigning as [" << i.key() << "]";
            } else {
                qDebug() << "ActionManager: new action " << i.value() << " - shortcut [" << i.key() << "] already assigned to another action " << actionForShortcut(i.key());
            }
        }
    }
}

void ActionManager::saveShortcuts() {
    settings->saveShortcuts(actionManager->shortcuts);
}



QString ActionManager::actionForScanCode(int code) {
    return actionManager->shortcuts[inputMap->keys()[code]];
}

QString ActionManager::actionForShortcut(QString keys) {
    return actionManager->shortcuts[keys];
}

bool ActionManager::invokeActionForShortcut(QString shortcut) {
    if(shortcuts.contains(shortcut)) {
        QMetaObject::invokeMethod(this,
                                  actionManager->shortcuts[shortcut].toLatin1().constData(),
                                  Qt::DirectConnection);
        return true;
    }
    return false;
}

void ActionManager::validateShortcuts() {
    for (auto i = shortcuts.begin(); i != shortcuts.end();) {
        if(!appActions->getMap().contains(i.value()))
            i=shortcuts.erase(i);
        else
            ++i;
    }
}

void ActionManager::readShortcuts() {
    settings->readShortcuts(shortcuts);
    actionManager->validateShortcuts();
}

// TODO: use some sort of enum instead of action string? this looks kinda retarded now
bool ActionManager::invokeAction(QString actionName) {
    if(actionList().contains(actionName)) {
        QMetaObject::invokeMethod(this,
                                  actionName.toLatin1().constData(),
                                  Qt::DirectConnection);
        return true;
    } else {
        qDebug() << "invalid action: " << actionName;
        return false;
    }
}

bool ActionManager::processEvent(QEvent *event) {
    return actionManager->invokeActionForShortcut(ShortcutBuilder::fromEvent(event));
}
