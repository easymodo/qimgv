#ifndef IMAGEANIMATED_H
#define IMAGEANIMATED_H

#include "image.h"
#include <QMovie>
#include <QTimer>

class ImageAnimated : public Image {
public:
    ImageAnimated(QString _path);
    ImageAnimated(ImageInfo *_info);
    ~ImageAnimated();

    QPixmap *getPixmap();
    const QImage* getImage();
    QMovie* getMovie();
    void load();
    void unload();
    int height();
    int width();
    QSize size();

    void rotate(int grad);
    void crop(QRect newRect);
public slots:
    void save();
    void save(QString destinationPath);

signals:
    void frameChanged(QPixmap*);

private:
    QSize mSize;
};

#endif // IMAGEANIMATED_H
