#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QObject>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMap>
#include <QDebug>
#include <QStringList>
#include "settings.h"

class ActionManager : public QObject
{
    Q_OBJECT
public:
    static ActionManager* getInstance();
    ~ActionManager();
    bool processEvent(QEvent*);
    void addShortcut(QString keys, QString action);
    void resetDefaults();
    QString actionForScanCode(int code);
    QString actionForShortcut(QString keys);
    const QStringList& actionList();
    const QMap<QString,QString>& allShortcuts();
    void removeShortcut(QString keys);
    const QStringList keys();
    void removeAll();
    QString keyForNativeScancode(int scanCode);

public slots:
    bool invokeAction(QString actionName);
private:
    explicit ActionManager(QObject *parent = 0);
    QMap<QString, QString> shortcuts;
    QMap<int, QString> keyMap;
    QMap<QString, Qt::KeyboardModifier> modMap;
    static void createActionList();
    static void initKeyMap();
    static void initModMap();
    static void initShortcuts();
    QStringList validActions;
    QString modifierKeys(QEvent *event);
    bool processWheelEvent(QWheelEvent *event);
    bool processMouseEvent(QMouseEvent *event);
    bool processKeyEvent(QKeyEvent *event);
    bool invokeActionForShortcut(QString action);
    void lock();
    void unlock();

signals:
    void open();
    void save();
    void openSettings();
    void crop();
    void setWallpaper();
    void nextImage();
    void prevImage();
    void fitAll();
    void fitWidth();
    void fitNormal();
    void toggleFitMode();
    void toggleFullscreen();
    void scrollUp();
    void scrollDown();
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

};

extern ActionManager *actionManager;

#endif // ACTIONMANAGER_H

