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

    QString currentFileName();
    QString currentFilePath();
    QString fullPath(QString fileName);

    int itemCount() const;
    int indexOf(QString fileName);
    QString fileNameAt(int index);
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

    bool setIndex(int index);
    bool setIndexAsync(int index);

    bool loaderBusy();

    std::shared_ptr<Image> itemAt(int index);

    std::shared_ptr<Image> getItemAt(int index);
    std::shared_ptr<Image> getItem(QString fileName);
    void updateItem(QString fileName, std::shared_ptr<Image> img);
    int currentIndex();
    void setSortingMode(SortingMode mode);
    SortingMode sortingMode();
signals:
    void fileRemoved(QString fileName, int index);
    void fileRenamed(QString from, int indexFrom, QString to, int indexTo);
    void fileAdded(QString fileName);
    void fileModified(QString fileName);
    void directoryChanged(QString);
    void sortingChanged();
    void indexChanged(int index);
    // returns current item
    void itemReady(std::shared_ptr<Image> img);
    void itemUpdated(std::shared_ptr<Image> img);

    void generateThumbnails(QList<int> indexes, int size);
    void thumbnailReady(std::shared_ptr<Thumbnail>);

private:
    DirectoryManager dirManager;
    Loader loader;
    Thumbnailer *thumbnailer;
    void preload(QString fileName);
    void trimCache();

    QString mCurrentFileName;

private slots:
    void onItemReady(std::shared_ptr<Image> img);
    void onSortingChanged();
    void onFileRemoved(QString fileName, int index);
};

#endif // DIRECTORYMODEL_H
