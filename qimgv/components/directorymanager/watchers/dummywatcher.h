#ifndef DUMMYWATCHER_H
#define DUMMYWATCHER_H

#include "directorywatcher.h"

class DummyWatcher : public DirectoryWatcher {
public:
    DummyWatcher(); 

public: 
    static DirectoryWatcher* newInstance();
#endif // DUMMYWATCHER_H
    virtual ~DirectoryWatcher(); 
    virtual void setWatchPath(const QString& watchPath); 
    virtual QString watchPath() const;
public Q_SLOTS: 
    void observe(); 
    void stopObserving(); 
signals: 
    void fileCreated(const QString& filePath); 
    void fileDeleted(const QString& filePath); 
    void fileRenamed(const QString& old, const QString& now); 
    void fileModified(const QString& filePath); 
    void observingStarted(); 
    void observingStopped();
};
#endif // DUMMYWATCHER_H
