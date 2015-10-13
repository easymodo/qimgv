#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QObject>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMap>
#include <QDebug>

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
private:
    explicit ActionManager(QObject *parent = 0);
    QMap<QString, QString> shortcuts;
    QMap<int, QString> keyMap;
    static void initKeyMap();
    static void initShortcuts();

    bool detectWheel(QWheelEvent *event);
    bool detectKeypress(QKeyEvent *event);
    void startAction(QString action);
signals:
    void open();
    void save();
    void settings();
    void crop();
    void setWallpaper();
    void nextImage();
    void prevImage();
    void toggleFitMode();
    void toggleMenuBar();
    void toggleFullscreen();
    void zoomIn();
    void zoomOut();
    void rotateLeft();
    void rotateRight();
    void exit();

public slots:
};

extern ActionManager *actionManager;

#endif // ACTIONMANAGER_H

