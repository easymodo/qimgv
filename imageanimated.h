#ifndef IMAGEANIMATED_H
#define IMAGEANIMATED_H

#include "settings.h"
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

    QPixmap* getPixmap();
    const QImage *getImage();
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
    void animationStop();

signals:
    void frameChanged(QPixmap*);

private slots:
    void nextFrame();

private:
    QMovie *movie;
    QTimer *timer;
    void startAnimationTimer();
};

#endif // IMAGEANIMATED_H
