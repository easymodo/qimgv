#include "imagestatic.h"
#include <time.h>
#include "utils/exifmetadata.h"

ImageStatic::ImageStatic(QString _path)
    : Image(_path)
{
    load();
    initPendingLossless();
}

ImageStatic::ImageStatic(std::unique_ptr<DocumentInfo> _info)
    : Image(std::move(_info))
{
    load();
    initPendingLossless();
}

ImageStatic::~ImageStatic() {
}

//load image data from disk
void ImageStatic::load() {
    if(isLoaded()) {
        return;
    }
    if(mDocInfo->mimeType().name() == "image/vnd.microsoft.icon")
        loadICO();
    else
        loadGeneric();
}


void ImageStatic::loadGeneric() {
    /* QImageReader::read() seems more reliable than just reading via QImage.
     * For example: "Invalid JPEG file structure: two SOF markers"
     * QImageReader::read() returns false, but still reads an image. Meanwhile QImage just fails.
     * I havent checked qimage's code, but it seems like it sees an exception
     * from libjpeg or whatever and just gives up on reading the file.
     *
     * tldr: qimage bad
     */
    QImageReader r(mPath, mDocInfo->format().toStdString().c_str());
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    r.setAllocationLimit(settings->memoryAllocationLimit());
#endif
    QImage *tmp = new QImage();
    r.read(tmp);
    std::unique_ptr<const QImage> img(tmp);
    img = ImageLib::exifRotated(std::move(img), mDocInfo.get()->exifOrientation());
    // scaling this format via qt results in transparent background
    // it rare enough so lets just convert it to the closest working thing
    if(img->format() == QImage::Format_Mono) {
        QImage *imgConverted = new QImage();
        *imgConverted = img->convertToFormat(QImage::Format_Grayscale8);
        image.reset(imgConverted);
    } else {
        // set image
        image = std::move(img);
    }
    mLoaded = true;
}

// TODO: move this out somewhere to use in other places
void ImageStatic::loadICO() {
    // Big brain code. It's mostly for small ico files so whatever. I'm not patching Qt for this.
    QIcon icon(mPath);
    QList<QSize> sizes = icon.availableSizes();
    QSize maxSize(0, 0);
    for(auto sz : sizes)
        if(maxSize.width() < sz.width())
            maxSize = sz;
    QPixmap iconPix = icon.pixmap(maxSize);
    std::unique_ptr<const QImage> img(new QImage(iconPix.toImage()));
    image = std::move(img);
    mLoaded = true;
}

QString ImageStatic::generateHash(QString str) {
    return QString(QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Md5).toHex());
}

// TODO: move saving to directorymodel
bool ImageStatic::save(QString destPath) {
    QString tmpPath = destPath + "_" + generateHash(destPath);
    QFileInfo fi(destPath);
    QString ext = fi.suffix();
    // png compression note from libpng
    // Note that tests have shown that zlib compression levels 3-6 usually perform as well
    // as level 9 for PNG images, and do considerably fewer caclulations
    int quality = 95;
    if(ext.compare("png", Qt::CaseInsensitive) == 0)
        quality = 30;
    else if(ext.compare("jpg", Qt::CaseInsensitive) == 0 || ext.compare("jpeg", Qt::CaseInsensitive) == 0)
        quality = settings->JPEGSaveQuality();

    bool backupExists = false, success = false, originalExists = false;

    // Qt writes the image without any of the metadata the source file
    // had, so it gets carried over by hand afterwards - read now, while
    // the source file is still there to read it from (saving over it is
    // the common case).
    ExifMetadata::Bundle metadata = ExifMetadata::read(mPath);

    if(QFile::exists(destPath))
        originalExists = true;

    // backup the original file if possible
    if(originalExists) {
        QFile::remove(tmpPath);
        if(!QFile::copy(destPath, tmpPath)) {
            qDebug() << "ImageStatic::save() - Could not create file backup.";
            return false;
        }
        backupExists = true;
    }
    // save file
    if(isEdited()) {
        success = imageEdited->save(destPath, ext.toStdString().c_str(), quality);
        image.swap(imageEdited);
        discardEditedImage();
    } else {
        success = image->save(destPath, ext.toStdString().c_str(), quality);
    }
    if(backupExists) {
        if(success) {
            // everything ok - remove the backup
            QFile file(tmpPath);
            file.remove();
        } else if(originalExists) {
            // revert on fail
            QFile::remove(mDocInfo->filePath());
            QFile::copy(tmpPath, mDocInfo->filePath());
            QFile::remove(tmpPath);
        }
    }
    if(success)
        ExifMetadata::writeTo(metadata, destPath, size());
    // Only when we wrote over the image's own file do the pending edits
    // become "already applied": a save-as elsewhere leaves this file (and
    // the displayed edits) untouched, so the queue has to stay put.
    if(success && destPath == mPath) {
        markPendingLosslessSaved();
        mDocInfo->refresh();
    }
    return success;
}

bool ImageStatic::save() {
    return save(mPath);
}

bool ImageStatic::saveLosslessBytes(QString destPath, const QByteArray &jpegBytes) {
    QString tmpPath = destPath + "_" + generateHash(destPath);
    bool backupExists = false, success = false, originalExists = false;

    if(QFile::exists(destPath))
        originalExists = true;

    // backup the original file if possible
    if(originalExists) {
        QFile::remove(tmpPath);
        if(!QFile::copy(destPath, tmpPath)) {
            qDebug() << "ImageStatic::saveLosslessBytes() - Could not create file backup.";
            return false;
        }
        backupExists = true;
    }
    // write the already-transformed JPEG bytes directly, no re-encoding
    QFile out(destPath);
    if(out.open(QIODevice::WriteOnly)) {
        success = (out.write(jpegBytes) == jpegBytes.size());
        out.close();
    }
    if(backupExists) {
        if(success) {
            // everything ok - remove the backup
            QFile file(tmpPath);
            file.remove();
        } else if(originalExists) {
            // revert on fail
            QFile::remove(mDocInfo->filePath());
            QFile::copy(tmpPath, mDocInfo->filePath());
            QFile::remove(tmpPath);
        }
    }
    // Only when we wrote over the image's own file do the pending edits
    // become "already applied": a save-as elsewhere leaves this file (and
    // the displayed edits) untouched, so the queue has to stay put.
    if(success && destPath == mPath) {
        markPendingLosslessSaved();
        mDocInfo->refresh();
        // The bytes we just wrote decode to exactly what imageEdited already
        // holds - the lossless path only ever runs on edits that are pure JPEG
        // transforms - so promote it to the source image instead of making the
        // caller reload from disk. That keeps the current zoom and pan, and
        // leaves both split panes looking at the same, no longer edited, Image.
        if(isEdited()) {
            image.swap(imageEdited);
            discardEditedImage();
        }
    }
    return success;
}

std::unique_ptr<QPixmap> ImageStatic::getPixmap() {
    std::unique_ptr<QPixmap> pix(new QPixmap());
    isEdited()?pix->convertFromImage(*imageEdited):pix->convertFromImage(*image, Qt::NoFormatConversion);
    return pix;
}

std::shared_ptr<const QImage> ImageStatic::getSourceImage() {
    return image;
}

std::shared_ptr<const QImage> ImageStatic::getImage() {
    return isEdited()?imageEdited:image;
}

int ImageStatic::height() {
    return isEdited()?imageEdited->height():image->height();
}

int ImageStatic::width() {
    return isEdited()?imageEdited->width():image->width();
}

QSize ImageStatic::size() {
    return isEdited()?imageEdited->size():image->size();
}

bool ImageStatic::setEditedImage(std::unique_ptr<const QImage> imageEditedNew) {
    if(imageEditedNew && imageEditedNew->width() != 0) {
        discardEditedImage();
        imageEdited = std::move(imageEditedNew);
        mEdited = true;
        return true;
    }
    return false;
}

bool ImageStatic::discardEditedImage() {
    if(imageEdited) {
        imageEdited.reset();
        mEdited = false;
        return true;
    }
    return false;
}

void ImageStatic::addPendingLosslessOp(LosslessJpegTransform::DihedralOp op) {
    if(mPendingLossless)
        mPendingLossless->addOp(op);
}

void ImageStatic::addPendingLosslessCrop(QRect rect, QSize sizeBeforeCrop) {
    if(mPendingLossless)
        mPendingLossless->addCrop(rect, sizeBeforeCrop);
}

void ImageStatic::invalidatePendingLossless() {
    mPendingLossless.reset();
}

void ImageStatic::initPendingLossless() {
    int orientation = mDocInfo->exifOrientation();
    if(orientation < 0 || orientation > 7)
        orientation = 0;
    mLosslessBaseOp = static_cast<LosslessJpegTransform::DihedralOp>(orientation);
    mPendingLossless = LosslessJpegTransform::PendingTransform(mLosslessBaseOp);
}

void ImageStatic::resetPendingLossless() {
    // mLosslessBaseOp is deliberately not re-derived from the EXIF
    // orientation here: mDocInfo's copy of it goes stale as soon as we
    // write the file ourselves, and by then the file needs no
    // orientation fixup at all anyway.
    mPendingLossless = LosslessJpegTransform::PendingTransform(mLosslessBaseOp);
}

void ImageStatic::markPendingLosslessSaved() {
    // The displayed pixels are now what this file holds, so there's
    // nothing left to replay onto it, and no orientation to fix up: the
    // EXIF tag was either normalized (lossless path) or dropped by Qt
    // (raster path).
    mLosslessBaseOp = LosslessJpegTransform::DihedralOp::None;
    mRawMcuSizeChecked = false; // a re-encode may have changed the subsampling
    mPendingLossless = LosslessJpegTransform::PendingTransform(mLosslessBaseOp);
}

bool ImageStatic::hasPendingLosslessChanges() const {
    return mPendingLossless
            && (mPendingLossless->op() != mLosslessBaseOp || mPendingLossless->crop().has_value());
}

LosslessJpegTransform::PendingTransform ImageStatic::pendingLossless() const {
    return mPendingLossless.value_or(LosslessJpegTransform::PendingTransform());
}

QSize ImageStatic::losslessMcuSize() {
#ifdef USE_TURBOJPEG
    if(!mPendingLossless)
        return QSize();
    if(!mRawMcuSizeChecked) {
        mRawMcuSizeChecked = true;
        mRawMcuSize = LosslessJpegTransform::mcuSize(mPath);
    }
    if(mRawMcuSize.isEmpty())
        return QSize();
    // a transposing op transposes the MCU grid along with the image
    // (a lossless transform transposes the subsampling factors too)
    if(LosslessJpegTransform::swapsAxes(mPendingLossless->op()))
        return QSize(mRawMcuSize.height(), mRawMcuSize.width());
    return mRawMcuSize;
#else
    return QSize();
#endif
}
