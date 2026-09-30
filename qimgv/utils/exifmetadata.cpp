#include "exifmetadata.h"

#ifdef USE_EXIV2
#include <exiv2/exiv2.hpp>
#include "utils/stuff.h"
#endif

namespace ExifMetadata {

#ifdef USE_EXIV2

struct Bundle::Data {
    Exiv2::ExifData exif;
    Exiv2::IptcData iptc;
    Exiv2::XmpData xmp;
};

Bundle::Bundle() : d(new Data) {}
Bundle::~Bundle() = default;
Bundle::Bundle(Bundle &&other) noexcept = default;
Bundle &Bundle::operator=(Bundle &&other) noexcept = default;

bool Bundle::isEmpty() const {
    return d->exif.empty() && d->iptc.empty() && d->xmp.empty();
}

// What an edit makes untrue about the image's own metadata.
static void applyEditFixups(Exiv2::ExifData &exif, QSize imageSize) {
    if(exif.empty())
        return;
    // the pixels are stored upright now, however they were rotated
    exif["Exif.Image.Orientation"] = uint16_t(1);
    if(!imageSize.isEmpty()) {
        exif["Exif.Photo.PixelXDimension"] = uint32_t(imageSize.width());
        exif["Exif.Photo.PixelYDimension"] = uint32_t(imageSize.height());
    }
    // the thumbnail would still show the image as it was before the edit
    Exiv2::ExifThumb(exif).erase();
}

Bundle read(const QString &filePath) {
    Bundle metadata;
    try {
        auto image = Exiv2::ImageFactory::open(toStdString(filePath));
        if(!image.get())
            return metadata;
        image->readMetadata();
        metadata.d->exif = image->exifData();
        metadata.d->iptc = image->iptcData();
        metadata.d->xmp  = image->xmpData();
    } catch(const std::exception &) {
        // best-effort: save without carrying the metadata over
    }
    return metadata;
}

void writeTo(const Bundle &metadata, const QString &filePath, QSize imageSize) {
    if(metadata.isEmpty())
        return;
    try {
        auto image = Exiv2::ImageFactory::open(toStdString(filePath));
        if(!image.get())
            return;
        Exiv2::ExifData exif = metadata.d->exif;
        applyEditFixups(exif, imageSize);
        image->setExifData(exif);
        image->setIptcData(metadata.d->iptc);
        image->setXmpData(metadata.d->xmp);
        image->writeMetadata();
    } catch(const std::exception &) {
        // best-effort: keep the saved file, just without its metadata
    }
}

void fixUpInPlace(QByteArray &jpegBytes, QSize imageSize) {
    try {
        auto image = Exiv2::ImageFactory::open(
            reinterpret_cast<const Exiv2::byte*>(jpegBytes.constData()), jpegBytes.size());
        if(!image.get())
            return;
        image->readMetadata();
        Exiv2::ExifData exif = image->exifData();
        if(exif.empty())
            return;
        applyEditFixups(exif, imageSize);
        image->setExifData(exif);
        image->writeMetadata();
        Exiv2::BasicIo &io = image->io();
        jpegBytes = QByteArray(reinterpret_cast<const char*>(io.mmap()), static_cast<int>(io.size()));
    } catch(const std::exception &) {
        // best-effort: keep the transformed bytes as they are
    }
}

#else // no Exiv2

struct Bundle::Data {};
Bundle::Bundle() = default;
Bundle::~Bundle() = default;
Bundle::Bundle(Bundle &&other) noexcept = default;
Bundle &Bundle::operator=(Bundle &&other) noexcept = default;
bool Bundle::isEmpty() const { return true; }

Bundle read(const QString &) { return Bundle(); }
void writeTo(const Bundle &, const QString &, QSize) {}
void fixUpInPlace(QByteArray &, QSize) {}

#endif // USE_EXIV2

} // namespace ExifMetadata
