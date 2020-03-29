#pragma once

#include <QObject>
#include "cache/cache.h"
#include "directorymanager/directorymanager.h"
#include "scaler/scaler.h"
#include "thumbnailer/thumbnailer.h"
#include "loader/loader.h"

enum FileOpResult {
    SUCCESS,
    DESTINATION_FILE_EXISTS,
    SOURCE_NOT_WRITABLE,
    DESTINATION_NOT_WRITABLE,
    SOURCE_DOES_NOT_EXIST,
    DESTINATION_DOES_NOT_EXIST,
    COPY_TO_SAME_DIR,
    OTHER_ERROR
};

class DirectoryModel : public QObject {
    Q_OBJECT
public:
    explicit DirectoryModel(QObject *parent = nullptr);
    ~DirectoryModel();

    Scaler *scaler;

    QString fullPath(QString fileName);

    void load(QString fileName, bool asyncHint);
    void preload(QString fileName);

    int itemCount() const;
    int indexOf(QString fileName);
    QString fileNameAt(int index);
    bool contains(QString fileName);
    void removeFile(QString fileName, bool trash, FileOpResult &result);
    bool isEmpty();
    QString nextOf(QString fileName);
    QString prevOf(QString fileName);
    QString first();
    QString last();
    QString absolutePath();
    QDateTime lastModified(QString fileName);
    void copyTo(QString destDir, QUrl srcUrl, FileOpResult &result);
    void moveTo(QString destDir, QUrl srcUrl, FileOpResult &result);
    void setDirectory(QString);

    void unload(int index);

    bool loaderBusy();

    std::shared_ptr<Image> itemAt(int index);

    std::shared_ptr<Image> getItemAt(int index);
    std::shared_ptr<Image> getItem(QString fileName);
    void updateItem(QString fileName, std::shared_ptr<Image> img);

    void setSortingMode(SortingMode mode);
    SortingMode sortingMode();
    bool forceInsert(QString fileName);

    QString directoryPath();
    void unload(QString fileName);
    bool isLoaded(int index);
    bool isLoaded(QString fileName);
    void reload(QString fileName);
    QString filePathAt(int index);
    void unloadExcept(QString fileName, bool keepNearby);
signals:
    void fileRemoved(QString fileName, int index);
    void fileRenamed(QString from, int indexFrom, QString to, int indexTo);
    void fileAdded(QString fileName);
    void fileModified(QString fileName);
    void fileModifiedInternal(QString fileName);
    void loaded(QString);
    void sortingChanged();
    void indexChanged(int oldIndex, int index);
    // returns current item
    void itemReady(std::shared_ptr<Image> img);
    void itemUpdated(QString fileName);

    void generateThumbnails(QList<int> indexes, int size, bool, bool);
    void thumbnailReady(std::shared_ptr<Thumbnail>);

private:
    DirectoryManager dirManager;
    Loader loader;
    Cache cache;
    Thumbnailer *thumbnailer;
    void trimCache(QString currentFileName);

private slots:
    void onItemReady(std::shared_ptr<Image> img);
    void onSortingChanged();
    void onFileAdded(QString fileName);
    void onFileRemoved(QString fileName, int index);
    void onFileRenamed(QString from, int indexFrom, QString to, int indexTo);
    void onFileModified(QString fileName);
};
