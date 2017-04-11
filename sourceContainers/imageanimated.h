#ifndef IMAGEANIMATED_H
#define IMAGEANIMATED_H

#include "image.h"
#include <QMovie>
#include <QTimer>

class ImageAnimated : public Image
{
    Q_OBJECT
public:
    ImageAnimated(QString _path);
    ImageAnimated(FileInfo *_info);
    ~ImageAnimated();

    QPixmap *getPixmap();
    const QImage* getImage();
    QMovie* getMovie();
    void load();
    void unload();
    int height();
    int width();
    QSize size();
    QPixmap* generateThumbnail(bool squared);

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
