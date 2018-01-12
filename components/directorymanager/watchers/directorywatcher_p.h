#ifndef DIRECTORYWATCHER_P_H
#define DIRECTORYWATCHER_P_H

#include "directorywatcher.h"
#include "watcherevent.h"
#include "watcherworker.h"

#include <QStringList>
#include <QThread>
#include <QtDebug>
#include <QDir>
#include <QTimerEvent>
#include <QDir>
#include <QVariant>
#include <QSharedPointer>

class DirectoryWatcherPrivate : public QObject {
    Q_OBJECT
public:
    explicit DirectoryWatcherPrivate(DirectoryWatcher* qq, WatcherWorker *w);

    bool isFileNeeded(const QString& fileName) const;
    
    DirectoryWatcher* q_ptr;
    QVector<QSharedPointer<WatcherEvent>> directoryEvents;
    QScopedPointer<WatcherWorker> worker;
    QScopedPointer<QThread> workerThread;
    QDir currentDirectory;

private:
    Q_DECLARE_PUBLIC(DirectoryWatcher)
};

#endif // DIRECTORYWATCHER_P_H
