#ifndef LINUXWATCHERWORKER_H
#define LINUXWATCHERWORKER_H

#include "linuxfsevent.h"
#include "../watcherworker.h"

class LinuxWorker : public WatcherWorker
{
    Q_OBJECT
public:
    LinuxWorker();

    void setDescriptor(int desc);
    void handleErrorCode(int code);

    virtual void run() override;

Q_SIGNALS:
    void fileEvent(LinuxFsEvent* event);

private:
    int fd;
};

#endif // LINUXWATCHERWORKER_H
