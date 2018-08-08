#include "imagelib.h"

QImage *ImageLib::rotated(const QImage *src, int grad) {
    QImage *img = new QImage();
    QTransform transform;
    transform.rotate(grad);
    *img = src->transformed(transform, Qt::SmoothTransformation);
    return img;
}
//------------------------------------------------------------------------------
QImage *ImageLib::rotated(std::shared_ptr<const QImage> src, int grad) {
    return rotated(src.get(), grad);
}
//------------------------------------------------------------------------------
QImage* ImageLib::cropped(const QImage *src, QRect newRect) {
    QImage *img = nullptr;
    if(src->rect().contains(newRect, false)) {
        img = new QImage(newRect.size(), src->format());
        *img = src->copy(newRect);
    } else {
        img = new QImage();
    }
    return img;
}
//------------------------------------------------------------------------------
QImage* ImageLib::cropped(std::shared_ptr<const QImage> src, QRect newRect) {
    return cropped(src.get(), newRect);
}
//------------------------------------------------------------------------------
QImage* ImageLib::flippedH(const QImage *src) {
    return new QImage(src->mirrored(true, false));
}
//------------------------------------------------------------------------------
QImage* ImageLib::flippedH(std::shared_ptr<const QImage> src) {
    return flippedH(src.get());
}
//------------------------------------------------------------------------------
QImage* ImageLib::flippedV(const QImage *src) {
    return new QImage(src->mirrored(false, true));
}
//------------------------------------------------------------------------------
QImage* ImageLib::flippedV(std::shared_ptr<const QImage> src) {
    return flippedV(src.get());
}
//------------------------------------------------------------------------------
std::unique_ptr<const QImage> ImageLib::exifRotated(std::unique_ptr<const QImage> src, int orientation) {
    switch(orientation) {
    case 1: {
        src.reset(ImageLib::flippedH(src.get()));
    } break;
    case 2: {
        src.reset(ImageLib::flippedV(src.get()));
    } break;
    case 3: {
        src.reset(ImageLib::flippedH(src.get()));
        src.reset(ImageLib::flippedV(src.get()));
    } break;
    case 4: {
        src.reset(ImageLib::rotated(src.get(), 90));
    } break;
    case 5: {
        src.reset(ImageLib::flippedH(src.get()));
        src.reset(ImageLib::rotated(src.get(), 90));
    } break;
    case 6: {
        src.reset(ImageLib::flippedV(src.get()));
        src.reset(ImageLib::rotated(src.get(), 90));
    } break;
    case 7: {
        src.reset(ImageLib::rotated(src.get(), -90));
    } break;
    default: {
    } break;
    }
    return src;
}
//------------------------------------------------------------------------------
std::unique_ptr<QImage> ImageLib::exifRotated(std::unique_ptr<QImage> src, int orientation) {
    switch(orientation) {
    case 1: {
        src.reset(ImageLib::flippedH(src.get()));
    } break;
    case 2: {
        src.reset(ImageLib::flippedV(src.get()));
    } break;
    case 3: {
        src.reset(ImageLib::flippedH(src.get()));
        src.reset(ImageLib::flippedV(src.get()));
    } break;
    case 4: {
        src.reset(ImageLib::rotated(src.get(), 90));
    } break;
    case 5: {
        src.reset(ImageLib::flippedH(src.get()));
        src.reset(ImageLib::rotated(src.get(), 90));
    } break;
    case 6: {
        src.reset(ImageLib::flippedV(src.get()));
        src.reset(ImageLib::rotated(src.get(), 90));
    } break;
    case 7: {
        src.reset(ImageLib::rotated(src.get(), -90));
    } break;
    default: {
    } break;
    }
    return src;
}
//------------------------------------------------------------------------------
/*

QImage *ImageLib::cropped(QRect newRect, QRect targetRes, bool upscaled) {
    QImage *cropped = new QImage(targetRes.size(), image->format());
    if(upscaled) {
        QImage temp = image->copy(newRect);
        *cropped = temp.scaled(targetRes.size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        QRect target(QPoint(0, 0), targetRes.size());
        target.moveCenter(cropped->rect().center());
        *cropped = cropped->copy(target);
    } else {
        newRect.moveCenter(image->rect().center());
        *cropped = image->copy(newRect);
    }
    return cropped;
}
*/

/* 0: nearest
 * 1: bilinear
 */

QImage* ImageLib::scaled(std::shared_ptr<const QImage> source, QSize destSize, int method) {
    switch (method) {
        case 0:
            return scaled_Qt(source, destSize, false);
        case 1:
            return scaled_Qt(source, destSize, true);
        /*case 2:
            return scale_FreeImage(source, destSize, FILTER_BICUBIC);
        case 3:
            return scale_FreeImage(source, destSize, FILTER_CATMULLROM);
        case 4:
            return scale_FreeImage(source, destSize, FILTER_LANCZOS3);
            */
        default:
            return scaled_Qt(source, destSize, true);
    }
}

QImage* ImageLib::scaled_Qt(std::shared_ptr<const QImage> source, QSize destSize, bool smooth) {
    QImage *dest = new QImage();
    Qt::TransformationMode mode = smooth ? Qt::SmoothTransformation : Qt::FastTransformation;
    *dest = source->scaled(destSize.width(), destSize.height(), Qt::IgnoreAspectRatio, mode);
    return dest;
}

/*
QImage* ImageLib::scale_FreeImage(const QImage *source, QSize destSize, FREE_IMAGE_FILTER filter) {
    FIBITMAP *fiOrig = FreeImage_ConvertFromRawBitsEx(false, (BYTE*)source->bits(),
                                                      FIT_BITMAP,
                                                      source->width(),
                                                      source->height(),
                                                      source->bytesPerLine(),
                                                      source->depth(),
                                                      FI_RGBA_RED_MASK,
                                                      FI_RGBA_GREEN_MASK,
                                                      FI_RGBA_BLUE_MASK);
    QImage *dest = new QImage(destSize, source->format());
    FIBITMAP *fiScaled = FreeImage_Rescale(fiOrig, dest->width(), dest->height(), filter);
    FreeImage_Unload(fiOrig);
    int pitch = FreeImage_GetPitch(fiScaled);
    int height = FreeImage_GetHeight(fiScaled);
    memcpy( dest->bits(), FreeImage_GetBits(fiScaled), pitch * height );
    FreeImage_Unload(fiScaled);
    return dest;
}
*/
