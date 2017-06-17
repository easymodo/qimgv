#include "watcherwindows.h"

WatcherWindows::WatcherWindows(QObject *parent) : QObject(parent) {
    mDir = "";
    mTimer.setInterval(500);
    mTimer.setSingleShot(true);
    connect(&mTimer, SIGNAL(timeout()), this, SLOT(loop()));
}

void WatcherWindows::setDir(QString _dir) {
    mTimer.stop();
    mDir = _dir;
    mTimer.start();
}

QString WatcherWindows::dir() {
    return mDir;
}

void WatcherWindows::setUpdateInterval(int msec) {
    mTimer.setInterval(msec);
}

void WatcherWindows::loop() {
    //qDebug() << "watcher timer";
    //mTimer.start();
}
