#ifndef DIRECTORYWATCHER_H
#define DIRECTORYWATCHER_H

#include <QObject>

class DirectoryWatcherPrivate;

class DirectoryWatcher : public QObject {
    Q_OBJECT
public:
    static DirectoryWatcher* newInstance();

    virtual ~DirectoryWatcher();

    QStringList fileFilters() const;
    void setFileFilters(const QStringList& filters);

    QStringList fileNames() const;

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

protected:
    DirectoryWatcher(DirectoryWatcherPrivate *ptr);
    DirectoryWatcherPrivate* d_ptr;

private:
    Q_DECLARE_PRIVATE(DirectoryWatcher)
};

#endif // DIRECTORYWATCHER_H
