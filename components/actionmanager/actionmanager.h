#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QObject>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMap>
#include <QDebug>
#include <QStringList>
#include "utils/inputmap.h"
#include "utils/actions.h"
#include "shortcutbuilder.h"
#include "settings.h"

class ActionManager : public QObject {
    Q_OBJECT
public:
    static ActionManager* getInstance();
    ~ActionManager();
    bool processEvent(QEvent*);
    void addShortcut(QString keys, QString action);
    void resetDefaults();
    QString actionForScanCode(int code);
    QString actionForShortcut(QString keys);
    QStringList actionList();
    const QMap<QString,QString>& allShortcuts();
    void removeShortcut(QString keys);
    const QStringList keys();
    void removeAllShortcuts();
    QString keyForNativeScancode(int scanCode);
    void resetDefaultsFromVersion(QVersionNumber lastVer);
    void saveShortcuts();

public slots:
    bool invokeAction(QString actionName);
private:
    explicit ActionManager(QObject *parent = 0);
    QMap<QString, QString> defaults, shortcuts; // <shortcut, action>

    static void initDefaults();
    static void initActions();
    static void initShortcuts();
    QString modifierKeys(QEvent *event);
    bool invokeActionForShortcut(QString action);
    void validateShortcuts();
    void readShortcuts();

signals:
    void open();
    void save();
    void saveAs();
    void openSettings();
    void crop();
    void setWallpaper();
    void nextImage();
    void prevImage();
    void fitWindow();
    void fitWidth();
    void fitNormal();
    void flipH();
    void flipV();
    void toggleFitMode();
    void toggleFullscreen();
    void scrollUp();
    void scrollDown();
    void scrollLeft();
    void scrollRight();
    void zoomIn();
    void zoomOut();
    void zoomInCursor();
    void zoomOutCursor();
    void resize();
    void rotateLeft();
    void rotateRight();
    void exit();
    void removeFile();
    void copyFile();
    void moveFile();
    void closeFullScreenOrExit();
    void jumpToFirst();
    void jumpToLast();
    void scriptPanel();
    void folderView();

};

extern ActionManager *actionManager;

#endif // ACTIONMANAGER_H

