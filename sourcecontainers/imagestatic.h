#ifndef QIMAGESTATIC_H
#define QIMAGESTATIC_H

#include <QImage>
#include <QSemaphore>
#include "image.h"

class ImageStatic : public Image {
public:
    ImageStatic(QString _path);
    ImageStatic(std::unique_ptr<DocumentInfo> _info);
    ~ImageStatic();

    std::unique_ptr<QPixmap> getPixmap();
    const QImage* getSourceImage();
    const QImage* getImage();
    int height();
    int width();
    QSize size();

    QImage *rotated(int grad);
    void rotate(int grad);
    QImage *cropped(QRect newRect, QRect targetRes, bool upscaled);

    bool setEditedImage(QImage *imageEditedNew);
    bool discardEditedImage();

public slots:
    void crop(QRect newRect);
    bool save();
    bool save(QString destPath);

private:
    void load();
    QImage *image, *imageEdited;
};

#endif // QIMAGESTATIC_H
