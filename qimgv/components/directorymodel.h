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

enum FileListSource { // rename? wip
    SOURCE_DIRECTORY,
    SOURCE_LIST
};

class DirectoryModel : public QObject {
    Q_OBJECT
public:
    explicit DirectoryModel(QObject *parent = nullptr);
    ~DirectoryModel();

    Scaler *scaler;

    QString fullPath(QString fileName) const;

    void load(QString fileName, bool asyncHint);
    void preload(QString fileName);

    int itemCount() const;
    int indexOf(QString fileName) const;
    QString fileNameAt(int index) const;
    bool contains(QString fileName) const;
    bool isEmpty() const;
    void removeFile(QString fileName, bool trash, FileOpResult &result);
    QString nextOf(QString fileName) const;
    QString prevOf(QString fileName) const;
    QString first() const;
    QString last() const;
    QDateTime lastModified(QString fileName) const;
    void copyTo(const QString destDirPath, const QFileInfo srcFile, FileOpResult &result);
    void moveTo(const QString destDirPath, const QFileInfo srcFile, FileOpResult &result);
    void setDirectory(QString);

    void unload(int index);

    bool loaderBusy() const;

    std::shared_ptr<Image> itemAt(int index);

    std::shared_ptr<Image> getItemAt(int index);
    std::shared_ptr<Image> getItem(QString fileName);
    void updateItem(QString fileName, std::shared_ptr<Image> img);

    void setSortingMode(SortingMode mode);
    SortingMode sortingMode() const;
    bool forceInsert(QString fileName);

    QString directoryPath() const;
    void unload(QString fileName);
    bool isLoaded(int index) const;
    bool isLoaded(QString fileName) const;
    void reload(QString fileName);
    QString filePathAt(int index) const;
    void unloadExcept(QString fileName, bool keepNearby);

signals:
    void fileRemoved(QString fileName, int index);
    void fileRenamed(QString from, int indexFrom, QString to, int indexTo);
    void fileAdded(QString fileName);
    void fileModified(QString fileName);
    void fileModifiedInternal(QString fileName);
    void loaded(QString);
    void sortingChanged(SortingMode);
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
    FileListSource fileListSource;
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
