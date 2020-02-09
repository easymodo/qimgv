#pragma once

#include <QImage>
#include <QImageWriter>
#include <QSemaphore>
#include <QCryptographicHash>
#include "image.h"
#include "utils/imagelib.h"
#include <settings.h>
#include <QIcon>

class ImageStatic : public Image {
public:
    ImageStatic(QString _path);
    ImageStatic(std::unique_ptr<DocumentInfo> _info);
    ~ImageStatic();

    std::unique_ptr<QPixmap> getPixmap();
    std::shared_ptr<const QImage> getSourceImage();
    std::shared_ptr<const QImage> getImage();

    int height();
    int width();
    QSize size();

    bool setEditedImage(std::unique_ptr<const QImage> imageEditedNew);
    bool discardEditedImage();

public slots:
    void crop(QRect newRect);
    bool save();
    bool save(QString destPath);

private:
    void load();
    std::shared_ptr<const QImage> image, imageEdited;
    void loadGeneric();
    void loadICO();
    QString generateHash(QString str);
};
