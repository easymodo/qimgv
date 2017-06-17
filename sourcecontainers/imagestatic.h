#ifndef QIMAGESTATIC_H
#define QIMAGESTATIC_H

#include <QImage>
#include <QSemaphore>
#include "image.h"

class ImageStatic : public Image {
public:
    ImageStatic(QString _path);
    ImageStatic(ImageInfo *_info);
    ~ImageStatic();

    QPixmap *getPixmap();
    const QImage* getImage();
    void load();
    int height();
    int width();
    QSize size();

    QImage *rotated(int grad);
    void rotate(int grad);
    QImage *cropped(QRect newRect, QRect targetRes, bool upscaled);

public slots:
    void crop(QRect newRect);
    void save();
    void save(QString destinationPath);

private:
    QImage *image;
    bool unloadRequested;
};

#endif // QIMAGESTATIC_H
