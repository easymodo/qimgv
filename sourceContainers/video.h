#ifndef VIDEO_H
#define VIDEO_H

#include "image.h"
#include "clip.h"
#include <QImage>
#include <QPixmap>
#include <QProcess>
#include <QFile>

class Video : public Image
{
    Q_OBJECT
public:
    Video(QString _path);
    Video(FileInfo *_info);
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
    QPixmap* generateThumbnail();

public slots:
    void crop(QRect newRect);
    void save();
    void save(QString destinationPath);


private:
    QPixmap *thumbnailStub();
    Clip *clip;
};

#endif // VIDEO_H
