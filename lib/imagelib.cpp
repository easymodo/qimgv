#include "imagelib.h"

ImageLib::ImageLib() {

}

void ImageLib::bilinearScale(QPixmap *dest, QPixmap *source, QSize destSize, bool smooth) {
    Qt::TransformationMode mode = smooth ? Qt::SmoothTransformation : Qt::FastTransformation;
    *dest = source->scaled(destSize.width(), destSize.height(), Qt::IgnoreAspectRatio, mode);
    delete source;
}
