#ifndef WINDOWSWATCHER_H
#define WINDOWSWATCHER_H

#include "../directorywatcher.h"

class WindowsWatcherPrivate;

class WindowsWatcher : public DirectoryWatcher {
    Q_OBJECT
public:
    explicit WindowsWatcher();
    explicit WindowsWatcher(const QString &path);
    virtual void setWatchPath(const QString& path);

private:
    Q_DECLARE_PRIVATE(WindowsWatcher)
};

#endif // WINDOWSWATCHER_H
