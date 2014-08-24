#ifndef QIMAGELOADER_H
#define QIMAGELOADER_H

#include <QFile>
#include <QString>
#include <QIODevice>
#include <QDebug>
#include <QMovie>
#include <QPixmap>
#include <QObject>

namespace Type
{
    enum
    {
        STATIC = 10,
        DYNAMIC,
        NONE
    };
};

namespace Format
{
    enum 
    {   // bytes in reversed order for litte endian systems
        JPG = 0xd8ff,
        PNG = 0x5089,
        GIF = 0x4947,
        //  bmp and other
    };
}

class Image : public QObject
{
    Q_OBJECT
public:
    Image();
    Image(QString path);
    ~Image();
    
    void* getSource() const;
    int getFormat() const;
    int getType() const;
    void loadImage(QString path);
    QString getPath() const;
private:
    int mFormat;
    int mType;
    QString mPath;
    void* mSource;
};

#endif // QIMAGELOADER_H
