#pragma once

#include <QObject>
#include <QDebug>
#include <QMutex>
#include <QClipboard>
#include <QDrag>
#include <QFileSystemModel>
#include <QDesktopServices>
#include "appversion.h"
#include "settings.h"
#include "components/directorymodel.h"
#include "components/directorypresenter.h"
#include "components/scriptmanager/scriptmanager.h"
#include "gui/mainwindow.h"
#include "utils/randomizer.h"

#ifdef __GLIBC__
#include <malloc.h>
#endif

struct State {
    bool hasActiveImage = false;
    QString currentFilePath = "";
};

enum MimeDataTarget {
    TARGET_CLIPBOARD,
    TARGET_DROP
};

class Core : public QObject {
    Q_OBJECT
public:
    Core();
    void showGui();

public slots:
    void updateInfoString();
    void loadPath(QString);

private:
    void initGui();
    void initComponents();
    void connectComponents();
    void initActions();
    void onUpdate();
    void onFirstRun();

    // ui stuff
    MW *mw;

    State state;
    bool loopSlideshow, slideshow;
    FolderEndAction folderEndAction;

    // components
    std::shared_ptr<DirectoryModel> model;

    DirectoryPresenter thumbPanelPresenter, folderViewPresenter;

    void rotateByDegrees(int degrees);
    void reset();
    void setDirectory(QString path);

    QDrag *mDrag;
    QMimeData *getMimeDataForImage(std::shared_ptr<Image> img, MimeDataTarget target);

    Randomizer randomizer;
    void syncRandomizer();

    void attachModel(DirectoryModel *_model);
    QString selectedFilePath();
    void guiSetImage(std::shared_ptr<Image> img);
    QTimer slideshowTimer;

    void startSlideshowTimer();
    void startSlideshow();
    void stopSlideshow();

    bool saveFile(const QString &filePath, const QString &newPath);
    bool saveFile(const QString &filePath);

    std::shared_ptr<ImageStatic> getEditableImage(const QString &filePath);
    QList<QString> currentSelection();

    template<typename... Args>
    void edit_template(bool save, const std::function<QImage*(std::shared_ptr<const QImage>, Args...)>& func, Args&&... as);

    bool copyFileConfirmation(QString src, QString dst);
    bool mergeDirConfirmation(QString src, QString dst);
    void interactiveCopy(QString path, QString destDirectory, bool &overwriteAllFiles, bool &mergeAllDirs, bool &cancelFlag);
private slots:
    void readSettings();
    void nextImage();
    void prevImage();
    void nextImageSlideshow();
    void jumpToFirst();
    void jumpToLast();
    void onModelItemReady(std::shared_ptr<Image>, const QString&);
    void onModelItemUpdated(QString fileName);
    void onModelSortingChanged(SortingMode mode);
    void onLoadFailed(const QString &path);
    void rotateLeft();
    void rotateRight();
    void close();
    void scalingRequest(QSize, ScalingFilter);
    void onScalingFinished(QPixmap* scaled, ScalerRequest req);
    void copyCurrentFile(QString destDirectory);
    void moveCurrentFile(QString destDirectory);
    void copyPathsTo(QList<QString> paths, QString destDirectory);
    void copyPathsTo(QList<QString> paths, QString destDirectory, bool overwriteAllFiles, bool mergeAllDirs);
    void interactiveCopy(QList<QString> paths, QString destDirectory, bool &overwriteAllFiles, bool &mergeAllDirs);
    void movePathsTo(QList<QString> paths, QString destDirectory);
    FileOpResult removeFile(QString fileName, bool trash);
    void onFileRemoved(QString filePath, int index);
    void onFileRenamed(QString fromPath, int indexFrom, QString toPath, int indexTo);
    void onFileAdded(QString filePath);
    void onFileModified(QString filePath);
    void showResizeDialog();
    void resize(QSize size);
    void flipH();
    void flipV();
    void crop(QRect rect);
    void cropAndSave(QRect rect);
    void discardEdits();
    void toggleCropPanel();
    void requestSavePath();
    void saveCurrentFile();
    void saveCurrentFileAs(QString);
    void runScript(const QString&);
    void removePermanent();
    void moveToTrash();
    void reloadImage();
    void reloadImage(QString fileName);
    void copyFileClipboard();
    void copyPathClipboard();
    void renameCurrentFile(QString newName);
    void sortBy(SortingMode mode);
    void sortByName();
    void sortByTime();
    void sortBySize();
    void showRenameDialog();
    void onDraggedOut();
    void onDraggedOut(QList<QString> paths);
    void onDropIn(const QMimeData *mimeData, QObject* source);
    void toggleShuffle();
    void onModelLoaded();
    void outputError(const FileOpResult &error) const;
    void showOpenDialog();
    void showInDirectory();
    void onDirectoryViewFileActivated(QString filePath);
    bool loadIndex(int index, bool async, bool preload);
    void enableDocumentView();
    void enableFolderView();
    void toggleFolderView();
    void toggleSlideshow();
    void onPlaybackFinished();
    void setFoldersDisplay(bool mode);
    void loadParentDir();
    void nextDirectory();
    void prevDirectory(bool selectLast);
    void prevDirectory();
};
