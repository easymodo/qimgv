#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include "imagecache.h"
#include "image.h"
#include "directorymanager.h"
#include "scrollarea.h"

class ImageLoader : public QObject
{
    Q_OBJECT
public:
    explicit ImageLoader(QObject *parent = 0);
    Image* loadNext();
    Image* loadPrev();
    DirectoryManager *dirManager;

private:
    ImageCache *cache;



signals:

public slots:

};

#endif // IMAGELOADER_H
