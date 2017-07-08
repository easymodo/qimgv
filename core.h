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

    void loadImage(QString filePath, bool blocking);

    // loads image in second thread
    void loadImage(QString);

    // loads image in main thread
    void loadImageBlocking(QString);

    // invalid position will be ignored
    bool openByIndex(int index);
    bool openByIndexBlocking(int index);

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
    NewLoader *imageLoader;
    DirectoryManager *dirManager;
    Cache2 *cache;
    Scaler *scaler;

    void rotateByDegrees(int degrees);

    void reset();
    bool setDirectory(QString newPath);
private slots:
    void readSettings();
    void slotNextImage();
    void slotPrevImage();
    void onLoadFinished(Image *img, int index);
    void onLoadStarted();
    void onLoadingTimeout();
    void clearCache();
    void stopPlayback();
    void rotateLeft();
    void rotateRight();
    void switchFitMode();
    void scalingRequest(QSize);
    void onScalingFinished(QPixmap* scaled, ScalerRequest req);
    void removeFile();
    void moveFile(QString destDirectory);
    void copyFile(QString destDirectory);
};

#endif // CORE_H
