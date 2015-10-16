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
        createActionList();
        initKeyMap();
        initShortcuts();
    }
    return actionManager;
}

// valid action names
void ActionManager::createActionList() {
    actionManager->validActions << "nextImage"
                                << "prevImage"
                                << "toggleFullscreen"
                                << "fitAll"
                                << "fitWidth"
                                << "fitNormal"
                                << "toggleFitMode"
                                << "toggleMenuBar"
                                << "rotateRight"
                                << "rotateLeft"
                                << "zoomIn"
                                << "zoomOut"
                                << "open"
                                << "save"
                                << "setWallpaper"
                                << "crop"
                                << "openSettings"
                                << "exit";
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
    actionManager->keyMap.insert(331, "Left");
    actionManager->keyMap.insert(333, "Right");
    actionManager->keyMap.insert(328, "Up");
    actionManager->keyMap.insert(336, "Down");
    actionManager->keyMap.insert(57, "Space");
    actionManager->keyMap.insert(28, "Return");
    actionManager->keyMap.insert(15, "Tab");
    actionManager->keyMap.insert(59, "F1");
    actionManager->keyMap.insert(60, "F2");
    actionManager->keyMap.insert(61, "F3");
    actionManager->keyMap.insert(62, "F4");
    actionManager->keyMap.insert(63, "F5");
    actionManager->keyMap.insert(64, "F6");
    actionManager->keyMap.insert(65, "F7");
    actionManager->keyMap.insert(66, "F8");
    actionManager->keyMap.insert(67, "F9");
    actionManager->keyMap.insert(68, "F10");
    actionManager->keyMap.insert(87, "F11");
    actionManager->keyMap.insert(88, "F12");
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
    actionManager->keyMap.insert(67, "F1");
    actionManager->keyMap.insert(68, "F2");
    actionManager->keyMap.insert(69, "F3");
    actionManager->keyMap.insert(70, "F4");
    actionManager->keyMap.insert(71, "F5");
    actionManager->keyMap.insert(72, "F6");
    actionManager->keyMap.insert(73, "F7");
    actionManager->keyMap.insert(74, "F8");
    actionManager->keyMap.insert(75, "F9");
    actionManager->keyMap.insert(76, "F10");
    actionManager->keyMap.insert(95, "F11");
    actionManager->keyMap.insert(96, "F12");
#endif
}

// fills actionManager->keyMap with defaults
// todo: check on windows
void ActionManager::initShortcuts() {
    settings->readShortcuts();
    if(actionManager->shortcuts.isEmpty()) {
        actionManager->resetDefaults();
    }
}

void ActionManager::addShortcut(QString keys, QString action) {
    if(actionManager->validActions.contains(action)) {
        actionManager->shortcuts.insert(keys, action);
    } else {
        qDebug() << "ActionManager: action " << action << " is invalid.";
    }
}

void ActionManager::removeShortcut(QString keys) {
    actionManager->shortcuts.remove(keys);
}

const QStringList& ActionManager::actionList() {
    return actionManager->validActions;
}

const QStringList ActionManager::keys() {
    QStringList list;
    QMapIterator<int,QString> i(actionManager->keyMap);
    while (i.hasNext()) {
        i.next();
        list << i.value();
    }
    list << "XButton1" << "XButton2" << "LMB" << "RMB" << "MiddleButton";
    return list;
}

const QMap<QString,QString>& ActionManager::allShortcuts() {
    return actionManager->shortcuts;
}

void ActionManager::removeAll() {
    shortcuts.clear();
}

void ActionManager::resetDefaults() {
    actionManager->shortcuts.clear();
    actionManager->addShortcut("Right", "nextImage");
    actionManager->addShortcut("Left", "prevImage");
    actionManager->addShortcut("XButton2", "nextImage");
    actionManager->addShortcut("XButton1", "prevImage");
    actionManager->addShortcut("WheelUp", "nextImage");
    actionManager->addShortcut("WheelDown", "prevImage");
    actionManager->addShortcut("F", "toggleFullscreen");
    actionManager->addShortcut("F11", "toggleFullscreen");
    actionManager->addShortcut("LMB_DoubleClick", "toggleFullscreen");
    actionManager->addShortcut("RMB_DoubleClick", "toggleFitMode");
    actionManager->addShortcut("MiddleButton", "toggleFitMode");
    actionManager->addShortcut("Space", "toggleFitMode");
    actionManager->addShortcut("A", "fitAll");
    actionManager->addShortcut("W", "fitWidth");
    actionManager->addShortcut("N", "fitNormal");
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
    actionManager->addShortcut("Ctrl+P", "openSettings");
    actionManager->addShortcut("Alt+X", "exit");
    actionManager->addShortcut("Ctrl+Q", "exit");
}

bool ActionManager::processWheelEvent(QWheelEvent *event) {
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

// Detects mouse button clicks only
// DoubleClick works only for LMB/RMB
// Otherwise treated as regular click
bool ActionManager::processMouseEvent(QMouseEvent *event) {
    QString keys;
    if(event->button() == Qt::XButton1) {
        keys = "XButton1";
    }
    if(event->button() == Qt::XButton2) {
        keys = "XButton2";
    }
    if(event->button() == Qt::LeftButton) {
        keys = "LMB";
    }
    if(event->button() == Qt::RightButton) {
        keys = "RMB";
    }
    if(event->button() == Qt::MiddleButton) {
        keys = "MiddleButton";
    }
    if(event->type() == QEvent::MouseButtonDblClick) {
        if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
            keys.append("_DoubleClick");
            actionManager->startAction(keys);
            return true;
        }
    }
    if(event->type() == QEvent::MouseButtonPress) {
        actionManager->startAction(keys);
        return true;
    }
    return false;
}

bool ActionManager::processKeyEvent(QKeyEvent *event) {
    if(event->type() == QEvent::KeyPress) {
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
    if(shortcuts.contains(shortcut)) {
        QMetaObject::invokeMethod(this,
                              actionManager->shortcuts[shortcut].toLatin1().constData(),
                              Qt::DirectConnection);
    }
}

bool ActionManager::processEvent(QEvent *event) {
    QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
    QWheelEvent *wheelEvent = dynamic_cast<QWheelEvent*>(event);
    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
    if(keyEvent) {
        return actionManager->processKeyEvent(keyEvent);
    } else if(wheelEvent) {
        actionManager->processWheelEvent(wheelEvent);
    } else if(mouseEvent) {
        actionManager->processMouseEvent(mouseEvent);
    }
    return false;
}
