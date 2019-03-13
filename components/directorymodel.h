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

    DirectoryManager dirManager;
    Cache cache;
    Scaler *scaler;
    Thumbnailer *thumbnailer;
    Loader loader;

    QString currentFileName;

signals:
    void fileRemoved(QString fileName, int index);
    void fileRenamed(QString from, QString to);
    void fileAdded(QString fileName);
    void fileModified(QString fileName);

public slots:
private:
};

#endif // DIRECTORYMODEL_H
