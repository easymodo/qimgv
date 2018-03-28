#ifndef THUMBNAIL_H
#define THUMBNAIL_H

#include <QString>
#include <QPixmap>

class Thumbnail {
public:
    Thumbnail();
    ~Thumbnail();
    QString name, resLabel;
    QPixmap *image;
    int size;
    bool isAnimated, isVideo;
};

#endif // THUMBNAIL_H
