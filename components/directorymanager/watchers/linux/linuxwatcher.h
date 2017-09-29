#ifndef LINUXDIRECTORYWATCHER_H
#define LINUXDIRECTORYWATCHER_H

#include "../directorywatcher.h"

class LinuxWatcherPrivate;

class LinuxWatcher : public DirectoryWatcher {
    Q_OBJECT
public:
    explicit LinuxWatcher();
    virtual ~LinuxWatcher();
    virtual void setWatchPath(const QString& p);

private:
    Q_DECLARE_PRIVATE(LinuxWatcher)
};

#endif // LINUXDIRECTORYWATCHER_H
