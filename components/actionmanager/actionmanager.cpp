#include "actionmanager.h"

ActionManager *actionManager = nullptr;

ActionManager::ActionManager(QObject *parent) : QObject(parent) {
}
//------------------------------------------------------------------------------
ActionManager::~ActionManager() {
    delete actionManager;
}
//------------------------------------------------------------------------------
ActionManager *ActionManager::getInstance() {
    if(!actionManager) {
        actionManager = new ActionManager();
        initDefaults();
        initShortcuts();
    }
    return actionManager;
}
//------------------------------------------------------------------------------
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
    actionManager->defaults.insert("delete", "moveToTrash");
    actionManager->defaults.insert("Shift+delete", "removeFile");
    actionManager->defaults.insert("C", "copyFile");
    actionManager->defaults.insert("M", "moveFile");
    actionManager->defaults.insert("home", "jumpToFirst");
    actionManager->defaults.insert("end", "jumpToLast");
    actionManager->defaults.insert("S", "s:TestScript");
    actionManager->defaults.insert("A", "s:TestScript2");
    actionManager->defaults.insert("Shift+Right", "seekVideo");
    actionManager->defaults.insert("Shift+Left", "seekBackVideo");
    actionManager->defaults.insert(",", "frameStep");
    actionManager->defaults.insert(".", "frameStepBack");
    actionManager->defaults.insert("Return", "folderView");
    actionManager->defaults.insert("backspace", "folderView");
}
//------------------------------------------------------------------------------
void ActionManager::initShortcuts() {
    actionManager->readShortcuts();
    if(actionManager->shortcuts.isEmpty()) {
        actionManager->resetDefaults();
    }
}
//------------------------------------------------------------------------------
void ActionManager::addShortcut(const QString &keys, const QString &action) {
    ActionType type = validateAction(action);
    if(type != ActionType::ACTION_INVALID) {
        actionManager->shortcuts.insert(keys, action);
    }
}
//------------------------------------------------------------------------------
void ActionManager::removeShortcut(const QString &keys) {
    actionManager->shortcuts.remove(keys);
}
//------------------------------------------------------------------------------
QStringList ActionManager::actionList() {
    return appActions->getList();
}
//------------------------------------------------------------------------------
const QMap<QString, QString> &ActionManager::allShortcuts() {
    return actionManager->shortcuts;
}
//------------------------------------------------------------------------------
void ActionManager::removeAllShortcuts() {
    shortcuts.clear();
}
//------------------------------------------------------------------------------
// Removes all shortcuts for specified action. Slow (reverse map lookup).
void ActionManager::removeAllShortcuts(QString actionName) {
    if(validateAction(actionName) == ActionType::ACTION_INVALID)
        return;

    for (auto i = shortcuts.begin(); i != shortcuts.end();) {
        if(i.value() == actionName)
            i = shortcuts.erase(i);
        else
            ++i;
    }
}
//------------------------------------------------------------------------------
QString ActionManager::keyForNativeScancode(quint32 scanCode) {
    if(inputMap->keys().contains(scanCode)) {
        return inputMap->keys()[scanCode];
    } else {
        return "";
    }
}
//------------------------------------------------------------------------------
void ActionManager::resetDefaults() {
    actionManager->shortcuts = actionManager->defaults;
}
//------------------------------------------------------------------------------
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
                qDebug() << "[ActionManager] new action " << i.value() << " - assigning as [" << i.key() << "]";
            } else {
                qDebug() << "[ActionManager] new action " << i.value() << " - shortcut [" << i.key() << "] already assigned to another action " << actionForShortcut(i.key());
            }
        }
    }
}
//------------------------------------------------------------------------------
void ActionManager::saveShortcuts() {
    settings->saveShortcuts(actionManager->shortcuts);
}
//------------------------------------------------------------------------------
QString ActionManager::actionForScanCode(quint32 code) {
    return actionManager->shortcuts[inputMap->keys()[code]];
}
//------------------------------------------------------------------------------
QString ActionManager::actionForShortcut(const QString &keys) {
    return actionManager->shortcuts[keys];
}

// returns first shortcut that is found
const QString ActionManager::shortcutForAction(QString action) {
    return shortcuts.key(action, "");
}

//------------------------------------------------------------------------------
bool ActionManager::invokeAction(const QString &actionName) {
    ActionType type = validateAction(actionName);
    if(type == ActionType::ACTION_NORMAL) {
        QMetaObject::invokeMethod(this, actionName.toLatin1().constData(), Qt::DirectConnection);
        return true;
    } else if(type == ActionType::ACTION_SCRIPT) {
        QString scriptName = actionName;
        scriptName.remove(0, 2); // remove the "s:" prefix
        emit runScript(scriptName);
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
bool ActionManager::invokeActionForShortcut(const QString &shortcut) {
    if(shortcut.isEmpty())
        return false;

    if(shortcuts.contains(shortcut)) {
        return invokeAction(actionManager->shortcuts[shortcut]);
    }
    return false;
}
//------------------------------------------------------------------------------
void ActionManager::validateShortcuts() {
    for (auto i = shortcuts.begin(); i != shortcuts.end();) {
        if(validateAction(i.value()) == ActionType::ACTION_INVALID)
            i = shortcuts.erase(i);
        else
            ++i;
    }
}
//------------------------------------------------------------------------------
inline
ActionType ActionManager::validateAction(const QString &actionName) {
    if(appActions->getMap().contains(actionName))
        return ActionType::ACTION_NORMAL;
    if(actionName.startsWith("s:")) {
        QString scriptName = actionName;
        scriptName.remove(0, 2);
        if(scriptManager->scriptExists(scriptName))
            return ActionType::ACTION_SCRIPT;
    }
    return ActionType::ACTION_INVALID;
}
//------------------------------------------------------------------------------
void ActionManager::readShortcuts() {
    settings->readShortcuts(shortcuts);
    actionManager->validateShortcuts();
}
//------------------------------------------------------------------------------
bool ActionManager::processEvent(QEvent *event) {
    return actionManager->invokeActionForShortcut(ShortcutBuilder::fromEvent(event));
}
//------------------------------------------------------------------------------
