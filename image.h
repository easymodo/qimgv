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

enum fileType { NONE, STATIC, GIF };

class Image : public QObject
{
    Q_OBJECT
public:
    Image(QString path);
    ~Image();

    const QImage* getImage();
    QMovie *getMovie();
    int getType();
    int ramSize();
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
    QImage* generateThumbnail();
    void unloadImage();
    FileInfo* getInfo();
    void attachInfo(FileInfo*);
public slots:
    void crop(QRect newRect);
    void save();
    void save(QString destinationPath);
private:
    FileInfo* info;
    bool loaded;
    QString path;
    const char* extension;
    fileType type;
    QImage *image;
    QMovie *movie;
    QSize resolution;
    bool inUseFlag;
    QMutex mutex;
    void detectType();
};

#endif // QIMAGELOADER_H
