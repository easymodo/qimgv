#pragma once

#include <QObject>
#include "cache/cache.h"
#include "directorymanager/directorymanager.h"
#include "scaler/scaler.h"
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

    void load(QString filePath, bool asyncHint);
    void preload(QString filePath);

    int fileCount() const;
    int dirCount() const;
    int indexOfFile(QString filePath) const;
    int indexOfDir(QString filePath) const;
    QString fileNameAt(int index) const;
    bool contains(QString filePath) const;
    bool isEmpty() const;
    void removeFile(QString filePath, bool trash, FileOpResult &result);
    QString nextOf(QString filePath) const;
    QString prevOf(QString filePath) const;
    QString first() const;
    QString last() const;
    QDateTime lastModified(QString filePath) const;
    void copyTo(const QString destDirPath, const QFileInfo srcFile, FileOpResult &result);
    void moveTo(const QString destDirPath, const QFileInfo srcFile, FileOpResult &result);
    void setDirectory(QString);

    void unload(int index);

    bool loaderBusy() const;

    std::shared_ptr<Image> getImageAt(int index);
    std::shared_ptr<Image> getImage(QString filePath);

    void updateImage(QString filePath, std::shared_ptr<Image> img);

    void setSortingMode(SortingMode mode);
    SortingMode sortingMode() const;
    bool forceInsert(QString filePath);

    QString directoryPath() const;
    void unload(QString filePath);
    bool isLoaded(int index) const;
    bool isLoaded(QString filePath) const;
    void reload(QString filePath);
    QString filePathAt(int index) const;
    void unloadExcept(QString filePath, bool keepNearby);
    const FSEntry &entryAt(int index) const;

    int totalCount() const;
    QString dirNameAt(int index) const;
    QString dirPathAt(int index) const;
signals:
    void fileRemoved(QString filePath, int index);
    void fileRenamed(QString fromPath, int indexFrom, QString toPath, int indexTo);
    void fileAdded(QString filePath);
    void fileModified(QString filePath);
    void fileModifiedInternal(QString filePath);
    void loaded(QString filePath);
    void sortingChanged(SortingMode);
    void indexChanged(int oldIndex, int index);
    // returns current item
    void imageReady(std::shared_ptr<Image> img);
    void imageUpdated(QString filePath);

private:
    DirectoryManager dirManager;
    Loader loader;
    Cache cache;
    FileListSource fileListSource;

private slots:
    void onImageReady(std::shared_ptr<Image> img);
    void onSortingChanged();
    void onFileAdded(QString filePath);
    void onFileRemoved(QString filePath, int index);
    void onFileRenamed(QString fromPath, int indexFrom, QString toPath, int indexTo);
    void onFileModified(QString filePath);
};
