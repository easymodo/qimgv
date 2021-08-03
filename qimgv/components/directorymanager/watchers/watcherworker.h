#pragma once

#include <QObject>

class WatcherWorker : public QObject
{
    Q_OBJECT
public:
    WatcherWorker();
    virtual void run() = 0;

public Q_SLOTS:
    void setRunning(bool running);

Q_SIGNALS:
    void error(const QString& errorMessage);
    void started();
    void finished();

protected:
    QAtomicInt isRunning;
};
