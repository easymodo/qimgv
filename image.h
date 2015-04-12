#ifndef QIMAGELOADER_H
#define QIMAGELOADER_H

#include <fileinfo.h>
#include <QString>
#include <QIODevice>
#include <QDebug>
#include <QMovie>
#include <QPixmap>
#include <QPixmapCache>
#include <QImage>
#include <QObject>
#include <QThread>
#include <QMutex>
#include <settings.h>

class Image : public QObject
{
    Q_OBJECT
public:
    Image(FileInfo*);
    ~Image();

    const QImage* getImage();
    QMovie *getMovie();
    int getType();
    int ramSize();
    FileInfo *getFileInfo() const;
    // size in kbytes
    qint64 getFileSize();
    QString getFileName();
    QDateTime getModifyDate();
    void loadImage();
    QString getPath();
    int height();
    int width();
    QSize size();
    bool compare(Image*);
    bool useFlag();
    void setUseFlag(bool);

    QImage *rotated(int grad);
    void rotate(int grad);
    bool isLoaded();
    QImage thumbnail();
    void unloadImage();
public slots:
    void crop(QRect newRect);
private:
    QImage *image;
    QMovie *movie;
    FileInfo *info;
    QSize resolution;
    bool inUseFlag;
    QMutex mutex;
};

#endif // QIMAGELOADER_H
