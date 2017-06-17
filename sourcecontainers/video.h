#ifndef VIDEO_H
#define VIDEO_H

#include <QImage>
#include <QPixmap>
#include <QProcess>
#include <QFile>
#include "image.h"
#include "clip.h"

class Video : public Image {
public:
    Video(QString _path);
    Video(ImageInfo *_info);
    ~Video();

    QPixmap* getPixmap();
    const QImage* getImage();
    Clip* getClip();    // getPixmap's video equivalent
    void load();
    void unload();
    int height();
    int width();
    QSize size();

    void rotate(int grad);

public slots:
    void crop(QRect newRect);
    void save();
    void save(QString destinationPath);


private:
    Clip *clip;
};

#endif // VIDEO_H
