#ifndef IMAGEANIMATED_H
#define IMAGEANIMATED_H

#include "settings.h"
#include "image.h"
#include <QMovie>

class ImageAnimated : public Image
{
    Q_OBJECT
public:
    ImageAnimated(QString _path);
    ~ImageAnimated();

    QPixmap* getPixmap();
    QImage *getImage();
    void load();
    void unload();
    int height();
    int width();
    QSize size();
    QPixmap* generateThumbnail();

    void rotate(int grad);
    void crop(QRect newRect);
public slots:
    void save();
    void save(QString destinationPath);
    void animationStart();
    void animationPause();
    void animationStop();

signals:
    void frameChanged(QPixmap*);

private slots:
    void frameChangedSlot(int frameNumber);

private:
    QMovie *movie;
};

#endif // IMAGEANIMATED_H
