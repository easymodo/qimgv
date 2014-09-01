#ifndef QIMAGELOADER_H
#define QIMAGELOADER_H

#include <fileinfo.h>
#include <QString>
#include <QIODevice>
#include <QDebug>
#include <QMovie>
#include <QPixmap>
#include <QPixmapCache>
//#include <QImage>
#include <QObject>

class Image : public QObject
{
    Q_OBJECT
public:
    Image();
    Image(FileInfo*);
    Image(QString path);
    ~Image();
    
    QPixmap* getPixmap() const;
    QMovie* getMovie() const;
    int getType() const;
    QDateTime getModifyDate() const;
    void loadImage(QString path);
    QString getPath() const;
    double getAspect();
    int height();
    int width();
    QSize size();
private:
    QString mPath;
    QPixmap* pixmap;
    QMovie* movie;
    FileInfo *info;
    QSize resolution;
    double aspectRatio;
};

#endif // QIMAGELOADER_H
