#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QWidget>
#include <QFileDialog>
#include "directorymanager.h"
#include "newloader.h"
#include "settings.h"
#include "sourceContainers/imageanimated.h"

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core();
    void open(QString);
    const ImageCache* getCache();
    QString getCurrentFilePath();

private:
    void initVariables();
    void connectSlots();
    NewLoader *imageLoader;
    DirectoryManager *dirManager;

    ImageAnimated* currentImageAnimated;
    Video* currentVideo;
    QMutex mutex;
    ImageCache *cache;

private slots:
    void onLoadStarted();
    void onLoadFinished(Image *img, int pos);
    void crop(QRect newRect);
    void releaseCurrentImage();

signals:
    void signalUnsetImage();
    void signalSetImage(QPixmap*);
    void infoStringChanged(QString);
    void slowLoading();
    void imageAltered(QPixmap*);
    void scalingFinished(QPixmap*);
    void frameChanged(QPixmap*);
    void thumbnailRequested(int);
    void thumbnailReady(int, const Thumbnail*);
    void cacheInitialized(int);
    void imageChanged(int);
    void startVideo();
    void stopVideo();
    void videoChanged(QString);

public slots:
    void init();
    void updateInfoString();
    void loadImage(QString);
    void loadImageBlocking(QString);
    void slotNextImage();
    void slotPrevImage();
    void setCurrentDir(QString);
    void rotateImage(int);

    void saveImage(QString path);
    void saveImage();
    void loadImageByPos(int pos);
    void rescaleForZoom(QSize newSize);
    void startAnimation();
    void stopAnimation();
};

#endif // CORE_H

