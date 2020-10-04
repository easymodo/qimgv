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
    bool infiniteScrolling, slideshow;

    // components
    std::shared_ptr<DirectoryModel> model;

    DirectoryPresenter thumbPanelPresenter, folderViewPresenter;

    void rotateByDegrees(int degrees);
    void reset();

    QDrag *mDrag;
    QMimeData *getMimeDataFor(std::shared_ptr<Image> img, MimeDataTarget target);

    Randomizer randomizer;
    void syncRandomizer();

    void attachModel(DirectoryModel *_model);
    QString selectedFilePath();
    void guiSetImage(std::shared_ptr<Image> img);
    QTimer slideshowTimer;

    void startSlideshowTimer();
    void stopSlideshow();
private slots:
    void readSettings();
    void nextImage();
    void prevImage();
    void nextImageSlideshow();
    void jumpToFirst();
    void jumpToLast();
    void onModelItemReady(std::shared_ptr<Image>);
    void onModelItemUpdated(QString fileName);
    void onModelSortingChanged(SortingMode mode);
    void onLoadFailed(QString path);
    void rotateLeft();
    void rotateRight();
    void close();
    void scalingRequest(QSize, ScalingFilter);
    void onScalingFinished(QPixmap* scaled, ScalerRequest req);
    void copyCurrentFile(QString destDirectory);
    void moveCurrentFile(QString destDirectory);
    void copyUrls(QList<QUrl> urls, QString destDirectory);
    void moveUrls(QList<QUrl> urls, QString destDirectory);
    void removeFile(QString fileName, bool trash);
    void onFileRemoved(QString filePath, int index);
    void onFileRenamed(QString fromPath, int indexFrom, QString toPath, int indexTo);
    void onFileAdded(QString filePath);
    void onFileModified(QString filePath);
    void showResizeDialog();
    void resize(QSize size);
    void flipH();
    void flipV();
    bool crop(QRect rect);
    void cropAndSave(QRect rect);
    void discardEdits();
    void toggleCropPanel();
    void requestSavePath();
    void saveImageToDisk();
    void saveImageToDisk(QString);
    void runScript(const QString&);
    void removeFilePermanent();
    void removeFilePermanent(QString fileName);
    void moveToTrash();
    void moveToTrash(QString fileName);
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
    void onDirectoryViewItemActivated(int index);
    bool loadIndex(int index, bool async, bool preload);
    void enableDocumentView();
    void enableFolderView();
    void toggleFolderView();
    void toggleSlideshow();
    void onPlaybackFinished();
    void setFoldersDisplay(bool mode);
};
