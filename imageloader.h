#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include "imagecache.h"
#include "image.h"
#include "directorymanager.h"
// #include "scrollarea.h"

class ImageLoader : public QObject
{
    Q_OBJECT
public:
    explicit ImageLoader(DirectoryManager*);
    Image* loadNext();
    Image* loadPrev();
    Image* load(QString file);
    DirectoryManager *dirManager;

private:
    ImageCache *cache;
    void loadFromCache(Image*& image);
signals:

public slots:

};

#endif // IMAGELOADER_H
