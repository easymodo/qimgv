#pragma once

#include <QString>
#include <QPixmap>
#include <memory>

class Thumbnail {
public:
    Thumbnail(QString _name, QString _info, int _size, std::shared_ptr<QPixmap> _pixmap);
    QString name();
    QString info();
    int size();
    bool hasAlphaChannel();
    std::shared_ptr<QPixmap> pixmap();
private:
    QString mName, mInfo;
    std::shared_ptr<QPixmap> mPixmap;
    int mSize;
    bool mHasAlphaChannel;
};
