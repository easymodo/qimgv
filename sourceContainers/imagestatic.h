#ifndef QIMAGESTATIC_H
#define QIMAGESTATIC_H

#include "image.h"
#include <QImage>
#include <QSemaphore>

class ImageStatic : public Image
{
    Q_OBJECT
public:
    ImageStatic(QString _path);
    ImageStatic(FileInfo *_info);
    ~ImageStatic();

    QPixmap *getPixmap();
    const QImage* getImage();
    void load();
    int height();
    int width();
    QSize size();

    QImage *rotated(int grad);
    void rotate(int grad);
    QPixmap *generateThumbnail();
    QImage *cropped(QRect newRect, QRect targetRes, bool upscaled);

public slots:
    void crop(QRect newRect);
    void save();
    void save(QString destinationPath);

private:
    QImage *image;
    QSemaphore *sem;
    bool unloadRequested;
};

#endif // QIMAGESTATIC_H
