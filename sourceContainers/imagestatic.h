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

    QPixmap* getPixmap();
    const QImage* getImage();
    void load();
    void unload();
    void unloadBlocking();
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
    QSemaphore *sem;
    bool unloadRequested;
    void lock();
    void unlock();
};

#endif // QIMAGESTATIC_H
