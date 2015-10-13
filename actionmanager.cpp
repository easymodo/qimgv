#include "actionmanager.h"

ActionManager *actionManager = NULL;

ActionManager::ActionManager(QObject *parent) : QObject(parent) {
}

ActionManager::~ActionManager() {
    delete actionManager;
}

ActionManager* ActionManager::getInstance() {
    if(!actionManager) {
        actionManager = new ActionManager();
        initKeyMap();
        initShortcuts();
    }
    return actionManager;
}

//TODO: update keymap for ps/2 keyboards
void ActionManager::initKeyMap() {
    actionManager->keyMap.clear();
#ifdef _WIN32
    actionManager->keyMap.insert(16, "Q");
    actionManager->keyMap.insert(17, "W");
    actionManager->keyMap.insert(18, "E");
    actionManager->keyMap.insert(19, "R");
    actionManager->keyMap.insert(20, "T");
    actionManager->keyMap.insert(21, "Y");
    actionManager->keyMap.insert(22, "U");
    actionManager->keyMap.insert(23, "I");
    actionManager->keyMap.insert(24, "O");
    actionManager->keyMap.insert(25, "P");

    actionManager->keyMap.insert(30, "A");
    actionManager->keyMap.insert(31, "S");
    actionManager->keyMap.insert(32, "D");
    actionManager->keyMap.insert(33, "F");
    actionManager->keyMap.insert(34, "G");
    actionManager->keyMap.insert(35, "H");
    actionManager->keyMap.insert(36, "J");
    actionManager->keyMap.insert(37, "K");
    actionManager->keyMap.insert(38, "L");

    actionManager->keyMap.insert(44, "Z");
    actionManager->keyMap.insert(45, "X");
    actionManager->keyMap.insert(46, "C");
    actionManager->keyMap.insert(47, "V");
    actionManager->keyMap.insert(48, "B");
    actionManager->keyMap.insert(49, "N");
    actionManager->keyMap.insert(50, "M");

    actionManager->keyMap.insert(57, "Space");
    actionManager->keyMap.insert(331, "Left");
    actionManager->keyMap.insert(333, "Right");
    actionManager->keyMap.insert(328, "Up");
    actionManager->keyMap.insert(336, "Down");
#elif defined __linux__
    actionManager->keyMap.insert(24, "Q");
    actionManager->keyMap.insert(25, "W");
    actionManager->keyMap.insert(26, "E");
    actionManager->keyMap.insert(27, "R");
    actionManager->keyMap.insert(28, "T");
    actionManager->keyMap.insert(29, "Y");
    actionManager->keyMap.insert(30, "U");
    actionManager->keyMap.insert(31, "I");
    actionManager->keyMap.insert(32, "O");
    actionManager->keyMap.insert(33, "P");
    actionManager->keyMap.insert(34, "[");
    actionManager->keyMap.insert(35, "]");

    actionManager->keyMap.insert(38, "A");
    actionManager->keyMap.insert(39, "S");
    actionManager->keyMap.insert(40, "D");
    actionManager->keyMap.insert(41, "F");
    actionManager->keyMap.insert(42, "G");
    actionManager->keyMap.insert(43, "H");
    actionManager->keyMap.insert(44, "J");
    actionManager->keyMap.insert(45, "K");
    actionManager->keyMap.insert(46, "L");
    actionManager->keyMap.insert(47, ";");
    actionManager->keyMap.insert(48, "'");

    actionManager->keyMap.insert(52, "Z");
    actionManager->keyMap.insert(53, "X");
    actionManager->keyMap.insert(54, "C");
    actionManager->keyMap.insert(55, "V");
    actionManager->keyMap.insert(56, "B");
    actionManager->keyMap.insert(57, "N");
    actionManager->keyMap.insert(58, "M");

    actionManager->keyMap.insert(59, ",");
    actionManager->keyMap.insert(60, ".");
    actionManager->keyMap.insert(61, "/");
    actionManager->keyMap.insert(58, "M");
    actionManager->keyMap.insert(113, "Left");
    actionManager->keyMap.insert(114, "Right");
    actionManager->keyMap.insert(111, "Up");
    actionManager->keyMap.insert(116, "Down");
    actionManager->keyMap.insert(65, "Space");
    actionManager->keyMap.insert(36, "Return");
    actionManager->keyMap.insert(23, "Tab");
#endif
}

// fills actionManager->keyMap with defaults
// todo: check on windows
void ActionManager::initShortcuts() {
    actionManager->resetDefaults();
}

void ActionManager::addShortcut(QString keys, QString action) {
    actionManager->shortcuts.insert(keys, action);
}

void ActionManager::resetDefaults() {
    actionManager->shortcuts.clear();
    actionManager->addShortcut("Right", "nextImage");
    actionManager->addShortcut("Left", "prevImage");
    actionManager->addShortcut("WheelUp", "nextImage");
    actionManager->addShortcut("WheelDown", "prevImage");
    actionManager->addShortcut("F", "toggleFullscreen");
    actionManager->addShortcut("Space", "toggleFitMode");
    actionManager->addShortcut("Ctrl+M", "toggleMenuBar");
    actionManager->addShortcut("Ctrl+R", "rotateRight");
    actionManager->addShortcut("Ctrl+L", "rotateLeft");
    actionManager->addShortcut("Ctrl+WheelDown", "zoomIn");
    actionManager->addShortcut("Ctrl+WheelUp", "zoomOut");
    actionManager->addShortcut("Up", "zoomIn");
    actionManager->addShortcut("Down", "zoomOut");
    actionManager->addShortcut("Ctrl+O", "open");
    actionManager->addShortcut("Ctrl+S", "save");
    actionManager->addShortcut("Ctrl+W", "setWallpaper");
    actionManager->addShortcut("X", "crop");
    actionManager->addShortcut("Ctrl+P", "settings");
    actionManager->addShortcut("Alt+X", "exit");
    actionManager->addShortcut("Ctrl+Q", "exit");
}

bool ActionManager::detectWheel(QWheelEvent *event) {
    QString keys;
    QString mods;

    if (event->angleDelta().ry() < 0) {
        keys = "WheelUp";
    } else if (event->angleDelta().ry() > 0) {
        keys = "WheelDown";
    }
    if(event->modifiers().testFlag(Qt::ControlModifier)) {
        mods.append("Ctrl+");
    }
    if(event->modifiers().testFlag(Qt::AltModifier)) {
        mods.append("Alt+");
    }
    if(event->modifiers().testFlag(Qt::ShiftModifier)) {
        mods.append("Shift+");
    }
    keys.prepend(mods);
    actionManager->startAction(keys);
    return true;
}

bool ActionManager::detectKeypress(QKeyEvent *event) {
    QString keys;
    keys = actionManager->keyMap[event->nativeScanCode()];
    if(!keys.isEmpty()) {
        QString mods;
        if(event->modifiers().testFlag(Qt::ControlModifier)) {
            mods.append("Ctrl+");
        }
        if(event->modifiers().testFlag(Qt::AltModifier)) {
            mods.append("Alt+");
        }
        if(event->modifiers().testFlag(Qt::ShiftModifier)) {
            mods.append("Shift+");
        }
        keys.prepend(mods);
        actionManager->startAction(keys);
        return true;
    }
    return false;
}

QString ActionManager::actionForScanCode(int code) {
    return actionManager->shortcuts[actionManager->keyMap[code]];
}

QString ActionManager::actionForShortcut(QString keys) {
    return actionManager->shortcuts[keys];
}

void ActionManager::startAction(QString shortcut) {
    QMetaObject::invokeMethod(this,
                              actionManager->shortcuts[shortcut].toLatin1().constData(),
                              Qt::DirectConnection);
}

bool ActionManager::processEvent(QEvent *event) {
    QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
    QWheelEvent *wheelEvent = dynamic_cast<QWheelEvent*>(event);
    if(keyEvent && keyEvent->type() == QEvent::KeyPress) {
        return actionManager->detectKeypress(keyEvent);
    } else if(wheelEvent) {
        actionManager->detectWheel(wheelEvent);
    }
    return false;
}



