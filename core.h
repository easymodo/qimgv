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
#include "gui/mainwindow.h"
#include "gui/viewers/viewerwidget.h"
#include "gui/viewers/imageviewer.h"
#include "gui/viewers/videoplayergl.h"

class Core : public QObject {
    Q_OBJECT
public:
    Core();
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
    MainWindow *mw;
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
    Scaler *scaler;

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
    void scalingRequest(QSize);
    void onScalingFinished(QPixmap* scaled, ScalerRequest req);
};

#endif // CORE_H
