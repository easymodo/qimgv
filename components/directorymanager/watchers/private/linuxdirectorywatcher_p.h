#ifndef LINUXDIRECTORYWATCHER_P_H
#define LINUXDIRECTORYWATCHER_P_H

#include "../linux/linuxwatcher.h"
#include "directorywatcher_p.h"

#include <errno.h>
#include <QDebug>
#include <QTimer>

class LinuxFsEvent;

class LinuxWatcherPrivate : public DirectoryWatcherPrivate
{
    Q_OBJECT
public:
    explicit LinuxWatcherPrivate(LinuxWatcher* qq = 0);

    int indexOfMoveEvent(uint cookie) const;

    int watcher;
    int watchObject;

    QVector<QSharedPointer<WatcherEvent>> moveEvents;

protected:
    virtual void timerEvent(QTimerEvent* timerEvent) override;

private slots:
    void dispatchFilesystemEvent(LinuxFsEvent *e);

private:
    Q_DECLARE_PUBLIC(LinuxWatcher)
};

#endif // LINUXDIRECTORYWATCHER_P_H
