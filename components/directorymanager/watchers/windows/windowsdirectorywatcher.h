#ifndef WINDOWSWATCHER_H
#define WINDOWSWATCHER_H

#include "../directorywatcher.h"

class WindowsDirectoryWatcherPrivate;

class WindowsDirectoryWatcher : public DirectoryWatcher
{
    Q_OBJECT
public:
    WindowsDirectoryWatcher();
    WindowsDirectoryWatcher(const QString &path);
    virtual ~WindowsDirectoryWatcher();
    virtual void setWatchPath(const QString& path);

private:
    Q_DECLARE_PRIVATE(WindowsDirectoryWatcher)

};

#endif // WINDOWSWATCHER_H
