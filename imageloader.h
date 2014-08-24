#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include "image.h"
#include "directorymanager.h"

class ImageLoader : public QObject
{
    Q_OBJECT
public:
    explicit ImageLoader(QObject *parent = 0);
    Image* loadNext();
    Image* loadPrev();
    FileInfo f;

signals:

public slots:

};

#endif // IMAGELOADER_H
