#include "imagelib.h"


/*QString type2str(int type) {
  QString r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}
*/

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

QImage* ImageLib::scaled(std::shared_ptr<const QImage> source, QSize destSize, ScalingFilter filter) {
    if(source->format() == QImage::Format_Indexed8 && filter > 1)
        filter = QI_FILTER_BILINEAR;
    switch (filter) {
        case QI_FILTER_NEAREST:
            return scaled_Qt(source, destSize, false);
        case QI_FILTER_BILINEAR:
            return scaled_Qt(source, destSize, true);
#ifdef USE_OPENCV
        case QI_FILTER_CV_BILINEAR_SHARPEN:
            return scaled_CV(source, destSize, cv::INTER_LINEAR, 0);
        case QI_FILTER_CV_CUBIC:
            return scaled_CV(source, destSize, cv::INTER_CUBIC, 0);
        case QI_FILTER_CV_CUBIC_SHARPEN:
            return scaled_CV(source, destSize, cv::INTER_CUBIC, 1);
#endif
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
#ifdef USE_OPENCV
// this probably leaks, needs checking
QImage* ImageLib::scaled_CV(std::shared_ptr<const QImage> source, QSize destSize, cv::InterpolationFlags filter, int sharpen) {
    QElapsedTimer t;
    t.start();
    cv::Mat srcMat = QtOcv::image2Mat_shared(*source.get());
    cv::Size destSizeCv(destSize.width(), destSize.height());
    QImage *dest = new QImage();
    if(destSize == source->size()) {
        // TODO: should this return a copy?
        //result.reset(new StaticImageContainer(std::make_shared<cv::Mat>(srcMat)));
    } else if(destSize.width() > source.get()->width()) { // upscale
        cv::Mat dstMat(destSizeCv, srcMat.type());
        cv::resize(srcMat, dstMat, destSizeCv, 0, 0, filter);
        *dest = QtOcv::mat2Image(dstMat);
    } else { // downscale
        float scale = (float)destSize.width() / source->width();
        if(scale < 0.5f && filter != cv::INTER_NEAREST) {
            if(filter == cv::INTER_CUBIC)
                sharpen = 1;
            filter = cv::INTER_AREA;
        }
        cv::Mat dstMat(destSizeCv, srcMat.type());
        cv::resize(srcMat, dstMat, destSizeCv, 0, 0, filter);
        if(!sharpen || filter == cv::INTER_NEAREST) {
            *dest = QtOcv::mat2Image(dstMat);
        } else {
            // todo: tweak this
            double amount = 0.25 * sharpen;
            // unsharp mask
            cv::Mat dstMat_sharpened;
            cv::GaussianBlur(dstMat, dstMat_sharpened, cv::Size(0, 0), 2);
            cv::addWeighted(dstMat, 1.0 + amount, dstMat_sharpened, -amount, 0, dstMat_sharpened);
            *dest = QtOcv::mat2Image(dstMat_sharpened);
        }
    }
    qDebug() << "Filter:" << filter << " sharpen=" << sharpen << " source size:" << source->size() << "->" << (float)destSize.width() / source->width() << ": " << t.elapsed() << " ms.";
    return dest;
}
#endif

