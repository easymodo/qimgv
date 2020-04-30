#pragma once

#include <QString>
#include <QPixmap>
#include <memory>

class Thumbnail {
public:
    Thumbnail(QString _name, QString _label, int _size, std::shared_ptr<QPixmap> _pixmap);
    QString name();
    QString label();
    int size();
    bool hasAlphaChannel();
    std::shared_ptr<QPixmap> pixmap();
private:
    QString mName, mLabel;
    std::shared_ptr<QPixmap> mPixmap;
    int mSize;
    bool mHasAlphaChannel;
};
