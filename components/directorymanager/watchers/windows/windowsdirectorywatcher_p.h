#ifndef WINDOWSWATCHER_P_H
#define WINDOWSWATCHER_P_H

#include <windows.h>

#include "directorywatcher_p.h"
#include "../windows/windowsdirectorywatcher.h"

#include <QObject>
#include <windows.h>
#include <QDebug>

class WindowsWatcherWorker : public WatcherWorker
{
    Q_OBJECT
public:
    WindowsWatcherWorker() : WatcherWorker() {}
    virtual ~WindowsWatcherWorker() {}
    void setDirectoryHandle(HANDLE hDirectory);
    virtual void run();
signals:
    void notifyEvent(PFILE_NOTIFY_INFORMATION);
private:
    HANDLE hDirectory;
    WCHAR buffer[1024];
    DWORD bytesReturned;
};

QString lastError();

class WindowsWatcherPrivate : public DirectoryWatcherPrivate
{
    Q_OBJECT
public:
    WindowsWatcherPrivate(WindowsWatcher* qq = 0);
    HANDLE requestDirectoryHandle(const QString& path);
    QString oldFileName;
public slots:
    void dispatchNotify(PFILE_NOTIFY_INFORMATION notify);
private:
    Q_DECLARE_PUBLIC(WindowsWatcher)
};

#endif // WINDOWSWATCHER_P_H
