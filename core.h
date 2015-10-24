#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QWidget>
#include <QDesktopWidget>
#include <QFileDialog>
#include "directorymanager.h"
#include "newloader.h"
#include "settings.h"
#include "sourceContainers/imageanimated.h"
#include "wallpapersetter.h"
#include "lib/stuff.h"
#include <time.h>

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core();
    void init();

    // full file path
    // returns empty string if no file open
    QString getCurrentFilePath();

public slots:
    void updateInfoString();

    // loads image in second thread
    void loadImage(QString);

    // loads image in main thread
    void loadImageBlocking(QString);

    // invalid position will be ignored
    void loadImageByPos(int pos);
    void slotNextImage();
    void slotPrevImage();

    // owerwrite image
    void saveImage();

    // save under specified name
    void saveImage(QString path);

    // changes directory; will reload cache & thumbnails
    // ignored if already in the same dir
    void setCurrentDir(QString);
    void rotateImage(int degrees);

    // TODO: screen and fit mode selection
    // crops/resizes current image to fill current screen
    // then sets it as wallpaper
    void setWallpaper(QRect wpRect);

    // TODO: move to another thread
    // makes a scaled copy of current image
    // and emits imageAltered(QPixmap*)
    void rescaleForZoom(QSize newSize);
    void startAnimation();
    void stopAnimation();

private:
    NewLoader *imageLoader;
    DirectoryManager *dirManager;
    ImageAnimated* currentImageAnimated;
    Video* currentVideo;
    QMutex mutex;
    ImageCache *cache;

    void initVariables();
    void connectSlots();

private slots:
    void onLoadStarted();

    // displays image and starts animation/video playback
    void onLoadFinished(Image *img, int pos);
    void crop(QRect newRect);

signals:
    void signalUnsetImage();
    void signalSetImage(QPixmap*);
    void infoStringChanged(QString);
    void slowLoading();
    void imageAltered(QPixmap*);
    void scalingFinished(QPixmap*);
    void frameChanged(QPixmap*);
    void thumbnailRequested(int);
    void thumbnailReady(int, Thumbnail*);
    void cacheInitialized(int);
    void imageChanged(int);
    void startVideo();
    void stopVideo();
    void videoChanged(QString);
};

#endif // CORE_H


