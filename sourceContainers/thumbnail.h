#ifndef THUMBNAIL_H
#define THUMBNAIL_H

#include <QString>
#include <QPixmap>

class Thumbnail {
public:
    Thumbnail();
    ~Thumbnail();
    QString name, label;
    QPixmap *image;
    int size;
};

#endif // THUMBNAIL_H
