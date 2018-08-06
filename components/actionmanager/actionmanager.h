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
#include "components/scriptmanager/scriptmanager.h"
#include "settings.h"

enum ActionType {
    ACTION_INVALID,
    ACTION_NORMAL,
    ACTION_SCRIPT
};

class ActionManager : public QObject {
    Q_OBJECT
public:
    static ActionManager* getInstance();
    ~ActionManager();
    bool processEvent(QEvent*);
    void addShortcut(const QString &keys, const QString &action);
    void resetDefaults();
    QString actionForScanCode(quint32 code);
    QString actionForShortcut(const QString &keys);
    const QString shortcutForAction(QString action);
    const QList<QString> shortcutsForAction(QString action);
    QStringList actionList();
    const QMap<QString,QString>& allShortcuts();
    void removeShortcut(const QString &keys);
    void removeAllShortcuts();
    void removeAllShortcuts(QString actionName);
    QString keyForNativeScancode(quint32 scanCode);
    void resetDefaultsFromVersion(QVersionNumber lastVer);
    void saveShortcuts();

public slots:
    bool invokeAction(const QString &actionName);
private:
    explicit ActionManager(QObject *parent = nullptr);
    QMap<QString, QString> defaults, shortcuts; // <shortcut, action>

    static void initDefaults();
    static void initActions();
    static void initShortcuts();
    QString modifierKeys(QEvent *event);
    bool invokeActionForShortcut(const QString &action);
    void validateShortcuts();
    void readShortcuts();
    ActionType validateAction(const QString &actionName);

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
    void folderView();
    void documentView();
    void runScript(const QString&);
    void pauseVideo();
    void seekVideo();
    void seekBackVideo();
    void frameStep();
    void frameStepBack();
    void toggleFolderView();
    void moveToTrash();
};

extern ActionManager *actionManager;

#endif // ACTIONMANAGER_H

