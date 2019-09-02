#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QDebug>
#include <QMutex>
#include <QClipboard>
#include <QDrag>
#include <malloc.h>
#include <QFileSystemModel>
#include "appversion.h"
#include "settings.h"
#include "components/directorymodel.h"
#include "components/directorypresenter.h"
#include "components/scriptmanager/scriptmanager.h"
#include "gui/mainwindow.h"

struct State {
    State() : hasActiveImage(false) {}
    bool hasActiveImage;
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
    MainWindow *mw;

    State state;
    bool infiniteScrolling;

    // components
    std::shared_ptr<DirectoryModel> model;

    DirectoryPresenter presenter;

    void rotateByDegrees(int degrees);
    void reset();
    void displayImage(std::shared_ptr<Image>);
    void loadDirectoryPath(QString);
    void loadImagePath(QString path, bool blocking);
    void trimCache();
    QDrag *mDrag;
    QMimeData *getMimeDataFor(std::shared_ptr<Image> img, MimeDataTarget target);

private slots:
    void readSettings();
    void nextImage();
    void prevImage();
    void jumpToFirst();
    void jumpToLast();
    void onModelItemReady(std::shared_ptr<Image>);
    void onModelItemUpdated(std::shared_ptr<Image>);
    void onLoadFailed(QString path); //
    void clearCache();
    void rotateLeft();
    void rotateRight();
    void close();
    void scalingRequest(QSize);
    void onScalingFinished(QPixmap* scaled, ScalerRequest req);
    void moveFile(QString destDirectory);
    void copyFile(QString destDirectory);
    void removeFile(QString fileName, bool trash);
    void onFileRemoved(QString fileName, int index);
    void onFileRenamed(QString from, int indexFrom, QString to, int indexTo);
    void onFileAdded(QString fileName);
    void onFileModified(QString fileName);
    void showResizeDialog();
    void resize(QSize size);
    void flipH();
    void flipV();
    void crop(QRect rect);
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
    void onDragOut();
    void onDropIn(const QMimeData *mimeData, QObject* source);
};

#endif // CORE_H
