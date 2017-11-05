#include "imagelib.h"

ImageLib::ImageLib() {

}

void ImageLib::scale(QImage *dest, const QImage *source, QSize destSize, bool smooth) {
    Qt::TransformationMode mode = smooth ? Qt::SmoothTransformation : Qt::FastTransformation;
    *dest = source->scaled(destSize.width(), destSize.height(), Qt::IgnoreAspectRatio, mode);
}

void ImageLib::scalehq(QImage *dest, const QImage *source, QSize destSize) {
    //qDebug() << "### src fmt: " << source->format() << "depth:" << source->depth() << "pitch:" << source->bytesPerLine();
    FIBITMAP *fiOrig = FreeImage_ConvertFromRawBitsEx(false, (BYTE*)source->bits(),
                                                      FIT_BITMAP,
                                                      source->width(),
                                                      source->height(),
                                                      source->bytesPerLine(),
                                                      source->depth(),
                                                      FI_RGBA_RED_MASK,
                                                      FI_RGBA_GREEN_MASK,
                                                      FI_RGBA_BLUE_MASK);
    // TODO: maybe fallback to some faster algo for very large sizes? this lags noticeably with high zoom levels
    FIBITMAP *fiScaled = FreeImage_Rescale(fiOrig, dest->width(), dest->height(), FILTER_CATMULLROM);
    FreeImage_Unload(fiOrig);
    int Width = FreeImage_GetWidth(fiScaled);
    int Pitch = FreeImage_GetPitch(fiScaled);
    int Height = FreeImage_GetHeight(fiScaled);
    int BPP = FreeImage_GetBPP(fiScaled);
    //qDebug() << FreeImage_GetPitch(fiScaled) << FreeImage_GetWidth(fiScaled) << FreeImage_GetBPP(fiScaled);
    //qDebug() << dest->bytesPerLine() << dest->width() << dest->depth();
    memcpy( dest->bits(), FreeImage_GetBits(fiScaled), Pitch * Height ); // broken with QImage::Format_Grayscale8
    FreeImage_Unload(fiScaled);
}
