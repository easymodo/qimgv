#ifndef THUMBNAIL_H
#define THUMBNAIL_H

#include <QString>
#include <QPixmap>
#include <memory>

class Thumbnail {
public:
    Thumbnail(QString _name, QString _label, int _size, std::shared_ptr<const QPixmap> _pixmap);
    QString name();
    QString label();
    int size();
    std::shared_ptr<const QPixmap> pixmap();
private:
    QString mName, mLabel;
    std::shared_ptr<const QPixmap> mPixmap;
    int mSize;
};

#endif // THUMBNAIL_H
