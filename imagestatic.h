#ifndef QIMAGESTATIC_H
#define QIMAGESTATIC_H

#include "settings.h"
#include "image.h"
#include <QImage>
#include <QMovie>

class ImageStatic : public Image
{
    Q_OBJECT
public:
    ImageStatic(QString _path);
    ~ImageStatic();

    QPixmap* getPixmap();
    QImage* getImage();
    void load();
    void unload();
    int height();
    int width();
    QSize size();

    QImage *rotated(int grad);
    void rotate(int grad);
    QPixmap* generateThumbnail();

public slots:
    void crop(QRect newRect);
    void save();
    void save(QString destinationPath);

private:
    QImage *image;
};

#endif // QIMAGESTATIC_H
