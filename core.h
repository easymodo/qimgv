#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QMutex>
#include <malloc.h>
#include "settings.h"
#include "components/directorymanager/directorymanager.h"
#include "components/loader/loader.h"
#include "components/scaler/scaler.h"
#include "components/thumbnailer/thumbnailer.h"
#include "gui/mainwindow.h"
#include "gui/viewers/viewerwidget.h"
#include "gui/viewers/imageviewer.h"
#include "gui/viewers/videoplayergl.h"

struct State {
    State() : currentIndex(0), hasActiveImage(false), isWaitingForLoader(false) {}
    int currentIndex;
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
    bool loadByIndexBlocking(int index);

signals:
    void imageIndexChanged(int);

private:
    void initGui();
    void initComponents();
    void connectComponents();
    void initActions();

    // ui stuff
    MainWindow *mw;
    ViewerWidget *viewerWidget;
    ImageViewer *imageViewer;
    VideoPlayerGL *videoPlayer;
    ThumbnailStrip *thumbnailPanelWidget;


    State state;
    QMutex mutex; // do i need this??
    QTimer *loadingTimer; // this is for loading message delay. TODO: replace with something better

    bool infiniteScrolling;

    // components
    Loader *loader;
    DirectoryManager *dirManager;
    Cache *cache;
    Scaler *scaler;
    Thumbnailer *thumbnailer;

    void rotateByDegrees(int degrees);

    void reset();
    bool setDirectory(QString newPath);
    void displayImage(Image *img);
    void loadDirectory(QString);
    void loadImage(QString path, bool blocking);
    void trimCache();
    void preload(int index);
private slots:
    void readSettings();
    void slotNextImage();
    void slotPrevImage();
    void jumpToFirst();
    void jumpToLast();
    void onLoadFinished(Image *img);
    void onLoadStarted();
    void onLoadingTimeout();
    void clearCache();
    void stopPlayback();
    void rotateLeft();
    void rotateRight();
    void closeBackgroundTasks();
    void close();
    void switchFitMode();
    void scalingRequest(QSize);
    void onScalingFinished(QPixmap* scaled, ScalerRequest req);
    void forwardThumbnail(Thumbnail*);
    void removeFile();
    void moveFile(QString destDirectory);
    void copyFile(QString destDirectory);
};

#endif // CORE_H
