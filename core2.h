#ifndef CORE2_H
#define CORE2_H

#include <QObject>
#include <QDebug>
#include "viewerwidget.h"
#include "viewers/imageviewer.h"
#include "viewers/videoplayergl.h"
#include "mainwindow2.h"
#include <QTimer>
#include "directorymanager.h"
#include "loader.h"
#include "settings.h"
#include <QMutex>

class Core2 : public QObject {
    Q_OBJECT
public:
    Core2();
    void showGui();

public slots:
    void updateInfoString();

    void loadImage(QString filePath, bool blocking);

    // loads image in second thread
    void loadImage(QString);

    // loads image in main thread
    void loadImageBlocking(QString);

    // invalid position will be ignored
    void openByIndex(int index);

signals:
    void imageIndexChanged(int);

private:
    void initGui();
    void initComponents();
    void connectComponents();
    void initActions();

    Image* currentImage();

    // ui stuff
    MainWindow2 *mw;
    ViewerWidget *viewerWidget;
    ImageViewer *imageViewer;
    VideoPlayerGL *videoPlayer;
    ThumbnailStrip *thumbnailPanelWidget;

    QMutex mutex;
    QTimer *loadingTimer; // this is for loading message delay. TODO: replace with something better

    int mCurrentIndex, mPreviousIndex, mImageCount, fitMode;
    bool infiniteScrolling;

    // re-think this?
    ImageAnimated* currentImageAnimated;
    Video* currentVideo;

    // components
    NewLoader *imageLoader;
    DirectoryManager *dirManager;
    ImageCache *cache;

    void rotateByDegrees(int degrees);

private slots:
    void readSettings();
    void slotNextImage();
    void slotPrevImage();
    void onLoadFinished(Image *img, int index);
    void onLoadStarted();
    void onLoadingTimeout();
    void initCache();
    void stopPlayback();
    void rotateLeft();
    void rotateRight();
};

#endif // CORE2_H
