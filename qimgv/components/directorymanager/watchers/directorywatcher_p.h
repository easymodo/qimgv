#ifndef DIRECTORYWATCHER_P_H
#define DIRECTORYWATCHER_P_H

#include "directorywatcher.h"
#include "watcherevent.h"
#include "watcherworker.h"

#include <QStringList>
#include <QThread>
#include <QtDebug>
#include <QTimerEvent>
#include <QVariant>
#include <QSharedPointer>

class DirectoryWatcherPrivate : public QObject {
    Q_OBJECT
public:
    explicit DirectoryWatcherPrivate(DirectoryWatcher* qq, WatcherWorker *w);

    DirectoryWatcher* q_ptr;
    QVector<QSharedPointer<WatcherEvent>> directoryEvents;
    QScopedPointer<WatcherWorker> worker;
    QScopedPointer<QThread> workerThread;
    QString currentDirectory;

private:
    Q_DECLARE_PUBLIC(DirectoryWatcher)
};

#endif // DIRECTORYWATCHER_P_H
