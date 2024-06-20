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
    actionManager->defaults.insert("WheelDown", "nextImage");
    actionManager->defaults.insert("WheelUp", "prevImage");
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
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+R", "rotateRight");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+L", "rotateLeft");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+WheelUp", "zoomInCursor");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+WheelDown", "zoomOutCursor");
    actionManager->defaults.insert("=", "zoomIn"); // [=+] key on the number row
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+=", "zoomIn");
    actionManager->defaults.insert("+", "zoomIn");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "++", "zoomIn");
    actionManager->defaults.insert("-", "zoomOut");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+-", "zoomOut");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+Down", "zoomOut");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+Up", "zoomIn");
    actionManager->defaults.insert("Up", "scrollUp");
    actionManager->defaults.insert("Down", "scrollDown");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+O", "open");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+S", "save");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+" + InputMap::keyNameShift() + "+S", "saveAs");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+W", "setWallpaper");
    actionManager->defaults.insert("X", "crop");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+P", "print");
    actionManager->defaults.insert(InputMap::keyNameAlt() + "+X", "exit");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+Q", "exit");
    actionManager->defaults.insert("Esc", "closeFullScreenOrExit");
    actionManager->defaults.insert("Del", "moveToTrash");
    actionManager->defaults.insert(InputMap::keyNameShift() + "+Del", "removeFile");
    actionManager->defaults.insert("C", "copyFile");
    actionManager->defaults.insert("M", "moveFile");
    actionManager->defaults.insert("Home", "jumpToFirst");
    actionManager->defaults.insert("End", "jumpToLast");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+Right", "seekVideoForward");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+Left", "seekVideoBackward");
    actionManager->defaults.insert(",", "frameStepBack");
    actionManager->defaults.insert(".", "frameStep");
    actionManager->defaults.insert("Enter", "folderView");
    actionManager->defaults.insert("Backspace", "folderView");
    actionManager->defaults.insert("F5", "reloadImage");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+C", "copyFileClipboard");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+" + InputMap::keyNameShift() + "+C", "copyPathClipboard");
    actionManager->defaults.insert("F2", "renameFile");
    actionManager->defaults.insert("RMB", "contextMenu");
    actionManager->defaults.insert("Menu", "contextMenu");
    actionManager->defaults.insert("I", "toggleImageInfo");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+`", "toggleShuffle");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+D", "showInDirectory");
    actionManager->defaults.insert("`", "toggleSlideshow");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+Z", "discardEdits");
    actionManager->defaults.insert(InputMap::keyNameShift() + "+Right", "nextDirectory");
    actionManager->defaults.insert(InputMap::keyNameShift() + "+Left", "prevDirectory");
    actionManager->defaults.insert(InputMap::keyNameShift() + "+F", "toggleFullscreenInfoBar");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+V", "pasteFile");

#ifdef __APPLE__
    actionManager->defaults.insert(InputMap::keyNameAlt() + "+Up", "zoomIn");
    actionManager->defaults.insert(InputMap::keyNameAlt() + "+Down", "zoomOut");
    actionManager->defaults.insert(InputMap::keyNameCtrl() + "+Comma", "openSettings");
#else
    actionManager->defaults.insert("P", "openSettings");
#endif

    //actionManager->defaults.insert("Backspace", "goUp"); // todo: shortcut scopes?
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
    }
    return "";
}
//------------------------------------------------------------------------------
void ActionManager::resetDefaults() {
    actionManager->shortcuts = actionManager->defaults;
}
//------------------------------------------------------------------------------
void ActionManager::resetDefaults(QString action) {
    actionManager->removeAllShortcuts(action);
    QMapIterator<QString, QString> i(defaults);
    while(i.hasNext()) {
        i.next();
        if(i.value() == action) {
            shortcuts.insert(i.key(), i.value());
            qDebug() << "[ActionManager] new action " << i.value() << " - assigning as [" << i.key() << "]";
        }
    }
}
//------------------------------------------------------------------------------
void ActionManager::adjustFromVersion(QVersionNumber lastVer) {
    // swap Ctrl-P & P
    if(lastVer < QVersionNumber(0,9,2)) {
        actionManager->resetDefaults("print");
        actionManager->resetDefaults("openSettings");
    }
    // swap WheelUp/WheelDown. derp
    if(lastVer < QVersionNumber(1,0,1)) {
        qDebug() << "[actionManager]: swapping WheelUp/WheelDown";
        QMapIterator<QString, QString> i(shortcuts);
        QMap<QString, QString> swapped;
        while(i.hasNext()) {
            i.next();
            QString key = i.key();
            if(key.contains("WheelUp"))
                key.replace("WheelUp", "WheelDown");
            else if(key.contains("WheelDown"))
                key.replace("WheelDown", "WheelUp");
            swapped.insert(key, i.value());
        }
        shortcuts = swapped;
    }
    // add new default actions
    QMapIterator<QString, QString> i(defaults);
    while(i.hasNext()) {
        i.next();
        if(appActions->getMap().value(i.value()) > lastVer) {
            if(!shortcuts.contains(i.key())) {
                shortcuts.insert(i.key(), i.value());
                qDebug() << "[ActionManager] new action " << i.value() << " - assigning as [" << i.key() << "]";
            } else if(i.value() != actionForShortcut(i.key())) {
                qDebug() << "[ActionManager] new action " << i.value() << " - shortcut [" << i.key() << "] already assigned to another action " << actionForShortcut(i.key());
            }
        }
    }
    // apply
    saveShortcuts();
}
//------------------------------------------------------------------------------
void ActionManager::saveShortcuts() {
    settings->saveShortcuts(actionManager->shortcuts);
}
//------------------------------------------------------------------------------
QString ActionManager::actionForShortcut(const QString &keys) {
    return actionManager->shortcuts[keys];
}

// returns first shortcut that is found
const QString ActionManager::shortcutForAction(QString action) {
    return shortcuts.key(action, "");
}

const QList<QString> ActionManager::shortcutsForAction(QString action) {
    return shortcuts.keys(action);
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
    if(!shortcut.isEmpty() && shortcuts.contains(shortcut)) {
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
bool ActionManager::processEvent(QInputEvent *event) {
    return actionManager->invokeActionForShortcut(ShortcutBuilder::fromEvent(event));
}
//------------------------------------------------------------------------------
