#ifndef DIRECTORYMODEL_H
#define DIRECTORYMODEL_H

#include <QObject>
#include "cache/cache.h"
#include "directorymanager/directorymanager.h"
#include "scaler/scaler.h"
#include "thumbnailer/thumbnailer.h"
#include "loader/loader.h"

class DirectoryModel : public QObject {
    Q_OBJECT
public:
    explicit DirectoryModel(QObject *parent = nullptr);
    ~DirectoryModel();

    Cache cache;
    Scaler *scaler;
    Thumbnailer *thumbnailer;
    Loader loader;

    QString currentFileName;

    int itemCount() const;
    int indexOf(QString fileName);
    QString fileNameAt(int index);
    QString fullFilePath(QString fileName);
    bool contains(QString fileName);
    bool removeFile(QString fileName, bool trash);
    bool isEmpty();
    QString nextOf(QString fileName);
    QString prevOf(QString fileName);
    QString first();
    QString last();
    QString absolutePath();
    QDateTime lastModified(QString fileName);
    bool copyTo(QString destDirectory, QString fileName);
    bool moveTo(QString destDirectory, QString fileName);
    void setDirectory(QString);

signals:
    void fileRemoved(QString fileName, int index);
    void fileRenamed(QString from, QString to);
    void fileAdded(QString fileName);
    void fileModified(QString fileName);
    void directoryChanged(QString);

public slots:
private:
    DirectoryManager dirManager;
};

#endif // DIRECTORYMODEL_H
