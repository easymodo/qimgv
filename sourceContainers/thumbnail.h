#ifndef THUMBNAIL_H
#define THUMBNAIL_H

#include <QString>
#include <QPixmap>

class Thumbnail
{
public:
    Thumbnail();
    QString name, label;
    QPixmap *image;
};

#endif // THUMBNAIL_H
