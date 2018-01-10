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
    ~Video();

    QPixmap* getPixmap();
    const QImage* getImage();
    Clip* getClip();    // getPixmap's video equivalent
    int height();
    int width();
    QSize size();

public slots:
    bool save();
    bool save(QString destPath);


private:
    void load();
    Clip *clip;
};

#endif // VIDEO_H
