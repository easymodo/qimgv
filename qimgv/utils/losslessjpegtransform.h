#pragma once

#include <QString>
#include <QByteArray>
#include <QRect>
#include <QSize>
#include <optional>

// Lossless JPEG rotate/flip/crop (jpegtran-style transform of the JPEG
// bitstream, no decode/recode) via libjpeg-turbo's TurboJPEG API.
// All functions are no-ops (return Failed / empty) when USE_TURBOJPEG
// is not defined, so callers can be written without #ifdef everywhere.
namespace LosslessJpegTransform {

// Mirrors Qt's QImageIOHandler::Transformation / EXIF Orientation values
// (0-7, same numbering, same bit meaning: bit0 = mirror horizontal,
// bit1 = flip vertical, bit2 = rotate 90 CW, applied in that order) so it
// can be composed directly with DocumentInfo::exifOrientation().
enum class DihedralOp {
    None              = 0,
    Mirror            = 1, // flip horizontal
    Flip              = 2, // flip vertical
    Rotate180         = 3,
    Rotate90          = 4,
    MirrorAndRotate90 = 5,
    FlipAndRotate90   = 6,
    Rotate270         = 7,
};

// Composes two operations expressed in the same space as above: the
// result is "apply `first` to the image, then apply `second` to that
// result". Used to combine a file's existing (baked-in) EXIF orientation
// with the rotation/flip the user additionally requested, since both are
// elements of the same 8-element dihedral group.
DihedralOp compose(DihedralOp first, DihedralOp second);

// The op that undoes `op`.
DihedralOp inverse(DihedralOp op);

// Whether `op` transposes width and height.
bool swapsAxes(DihedralOp op);

// Maps a rectangle through `op`. `r` is in the coordinate space of an
// image of size `srcSize`; the result is in the coordinate space of that
// image after `op` has been applied to it.
QRect mapRect(QRect r, DihedralOp op, QSize srcSize);

// Everything the user has done to an image since it was loaded (or last
// saved), in a form that can be replayed losslessly on the JPEG file.
//
// Any sequence of crops and rotates/flips collapses into a single crop
// followed by a single rotate/flip: crops and dihedral ops commute once
// the crop rectangle is mapped through the op, two crops merge into one,
// and two ops compose into one. So instead of replaying a list of steps,
// this keeps just the canonical pair - one crop rectangle in the *file's*
// own coordinates, plus the op to apply after it - which is also exactly
// what a JPEG can be transformed with in one crop pass plus one op pass.
//
// The starting op is the file's EXIF orientation, because qimgv displays
// images already rotated to upright (see ImageLib::exifRotated), so every
// crop/rotate/flip the user asks for is relative to that upright view.
class PendingTransform {
public:
    PendingTransform() = default;
    explicit PendingTransform(DihedralOp baseOrientation) : mOp(baseOrientation) {}

    void addOp(DihedralOp op) { mOp = compose(mOp, op); }

    // `cropInCurrentCoords` is a rectangle in the currently displayed
    // image, whose size is `currentSize`.
    void addCrop(QRect cropInCurrentCoords, QSize currentSize) {
        QRect local = mapRect(cropInCurrentCoords, inverse(mOp), currentSize);
        QPoint origin = mCrop ? mCrop->topLeft() : QPoint(0, 0);
        mCrop = QRect(origin + local.topLeft(), local.size());
    }

    DihedralOp op() const { return mOp; }
    std::optional<QRect> crop() const { return mCrop; }

private:
    DihedralOp mOp = DihedralOp::None;
    std::optional<QRect> mCrop;
};

enum class Result {
    Ok,        // transform applied, outJpegBytes holds the result
    NotAligned,// dimensions not MCU-aligned, transform needs a crop first
    Failed,    // could not read/transform the file (I/O or format error)
};

#ifdef USE_TURBOJPEG

// MCU block size (in pixels) for the given JPEG file, derived from its
// chroma subsampling (4:4:4 -> 8x8, 4:2:2 -> 16x8, 4:2:0 -> 16x16, etc.)
// Returns an empty/invalid QSize on failure.
QSize mcuSize(const QString &filePath);

// Attempts a perfect (no data loss, no implicit trim) lossless transform.
// `cropRegion`, if set, is in the *original* (pre-transform) image's pixel
// coordinates and must already be MCU-aligned on its left/top edge.
// Returns NotAligned (no file written) if the operation would require
// dropping partial MCU blocks - callers should then either give up, fall
// back to a lossy raster save, or call transformWithSymmetricCrop().
Result tryTransform(const QString &filePath, DihedralOp op,
                     std::optional<QRect> cropRegion, QByteArray &outJpegBytes);

// Same as tryTransform(), but if the geometry isn't MCU-aligned, first
// trims the minimal number of pixels needed to reach alignment (always
// from the right/bottom edge, since that's the only edge that can be
// trimmed without discarding a whole extra MCU block - equivalent to
// jpegtran's -trim). Always lossless; may remove a few extra pixels
// beyond what was requested in `requestedCrop`. Returns an empty
// QByteArray on failure.
QByteArray transformWithAlignmentTrim(const QString &filePath, DihedralOp op,
                                       std::optional<QRect> requestedCrop);

#endif // USE_TURBOJPEG

} // namespace LosslessJpegTransform
