#ifndef WINDOWSWATCHER_P_H
#define WINDOWSWATCHER_P_H

#include "../windows/windowswatcher.h"
#include "../directorywatcher_p.h"

#include <QObject>
#include <windows.h>
#include <QDebug>

QString lastError();

class WindowsWatcherPrivate : public DirectoryWatcherPrivate {
    Q_OBJECT
public:
    explicit WindowsWatcherPrivate(WindowsWatcher* qq = 0);
    HANDLE requestDirectoryHandle(const QString& path);
    QString oldFileName;

public slots:
    void dispatchNotify(PFILE_NOTIFY_INFORMATION notify);

private:
    Q_DECLARE_PUBLIC(WindowsWatcher)
};

#endif // WINDOWSWATCHER_P_H
