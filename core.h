#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QDebug>
#include <QMutex>
#include <QClipboard>
#include <malloc.h>
#include "appversion.h"
#include "settings.h"
#include "components/directorymodel.h"
#include "components/directorypresenter.h"
#include "components/scriptmanager/scriptmanager.h"
#include "gui/mainwindow.h"

struct State {
    State() : hasActiveImage(false), isWaitingForLoader(false) {}
    bool hasActiveImage, isWaitingForLoader;
};

class Core : public QObject {
    Q_OBJECT
public:
    Core();
    void showGui();

public slots:
    void updateInfoString();

    void loadByPath(QString filePath, bool blocking);

    // loads image in second thread
    void loadByPath(QString);

    // loads image in main thread
    void loadByPathBlocking(QString);

    // invalid position will be ignored
    bool loadByIndex(int index);

signals:
    void currentIndexChanged(int);

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
    bool setDirectory(QString newPath);
    void displayImage(Image *img);
    void loadDirectory(QString);
    void loadImage(QString path, bool blocking);
    void trimCache();
    void preload(QString fileName);

private slots:
    void readSettings();
    void nextImage();
    void prevImage();
    void jumpToFirst();
    void jumpToLast();
    void onLoadFinished(std::shared_ptr<Image> img);
    void onLoadFailed(QString path);
    void onLoadStarted();
    void clearCache();
    void rotateLeft();
    void rotateRight();
    void closeBackgroundTasks();
    void close();
    void scalingRequest(QSize);
    void onScalingFinished(QPixmap* scaled, ScalerRequest req);
    void moveFile(QString destDirectory);
    void copyFile(QString destDirectory);
    void removeFile(QString fileName, bool trash);
    void onFileRemoved(QString fileName, int index);
    void onFileRenamed(QString from, QString to);
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
    void renameRequested();
};

#endif // CORE_H
