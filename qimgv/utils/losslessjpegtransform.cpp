#include "losslessjpegtransform.h"

namespace LosslessJpegTransform {

// Composition table for the 8-element dihedral group, in the same
// numbering as Qt's QImageIOHandler::Transformation (and EXIF
// Orientation - 1): bit0 = mirror horizontal, bit1 = flip vertical,
// bit2 = rotate 90 CW, ops applied in that order (mirror, then flip,
// then rotate90), matching ImageLib::exifRotated()'s switch bodies.
// table[first][second] = the single equivalent op of "apply `first`,
// then apply `second`". Derived and verified programmatically (not
// hand-guessed) by tracking where the 4 corners of a non-square test
// rectangle end up under each op and its compositions.
static const int kComposeTable[8][8] = {
    {0, 1, 2, 3, 4, 5, 6, 7},
    {1, 0, 3, 2, 5, 4, 7, 6},
    {2, 3, 0, 1, 6, 7, 4, 5},
    {3, 2, 1, 0, 7, 6, 5, 4},
    {4, 6, 5, 7, 3, 1, 2, 0},
    {5, 7, 4, 6, 2, 0, 3, 1},
    {6, 4, 7, 5, 1, 3, 0, 2},
    {7, 5, 6, 4, 0, 2, 1, 3},
};

DihedralOp compose(DihedralOp first, DihedralOp second) {
    return static_cast<DihedralOp>(kComposeTable[static_cast<int>(first)][static_cast<int>(second)]);
}

DihedralOp inverse(DihedralOp op) {
    // in this group every reflection is its own inverse and so is the
    // 180 degree rotation; only the two 90 degree rotations pair up
    switch(op) {
        case DihedralOp::Rotate90:  return DihedralOp::Rotate270;
        case DihedralOp::Rotate270: return DihedralOp::Rotate90;
        default:                    return op;
    }
}

bool swapsAxes(DihedralOp op) {
    return static_cast<int>(op) & 4; // the rotate-90 bit
}

QRect mapRect(QRect r, DihedralOp op, QSize srcSize) {
    int bits = static_cast<int>(op);
    // applied in the same order the op's bits are defined in: mirror,
    // then flip, then rotate 90 clockwise
    if(bits & 1)
        r.moveLeft(srcSize.width() - (r.x() + r.width()));
    if(bits & 2)
        r.moveTop(srcSize.height() - (r.y() + r.height()));
    if(bits & 4) // transposes the canvas: (x,y) -> (H-1-y, x)
        r = QRect(srcSize.height() - (r.y() + r.height()), r.x(), r.height(), r.width());
    return r;
}

} // namespace LosslessJpegTransform

#ifdef USE_TURBOJPEG

#include <QFile>
#include <cstring>
#include <turbojpeg.h>

#include "utils/exifmetadata.h"

namespace LosslessJpegTransform {

// Maps a DihedralOp to the equivalent TJXOP_* constant. Verified
// programmatically together with kComposeTable (same corner-tracking
// method), not derived by hand.
static int toTJXOP(DihedralOp op) {
    static const int map[8] = {
        TJXOP_NONE,       // None
        TJXOP_HFLIP,      // Mirror
        TJXOP_VFLIP,      // Flip
        TJXOP_ROT180,     // Rotate180
        TJXOP_ROT90,      // Rotate90
        TJXOP_TRANSVERSE, // MirrorAndRotate90
        TJXOP_TRANSPOSE,  // FlipAndRotate90
        TJXOP_ROT270,     // Rotate270
    };
    return map[static_cast<int>(op)];
}

// Whether `op` requires the working region's width/height (respectively)
// to be an exact multiple of the MCU block size for the transform to be
// lossless, in *source* (pre-transform) coordinates. Not derivable from
// the op's bit pattern alone - verified empirically against
// libjpeg-turbo/jpegtran 2.1.5 with jpegtran -<op> -perfect on images
// with only one misaligned axis at a time: Rotate90 and Rotate270 each
// constrain a single axis, and a different one from each other; a plain
// diagonal Transpose needs no alignment at all, while its opposite
// diagonal (Transverse) needs both, same as Rotate180.
static void requiredAlignment(DihedralOp op, bool &needsWidthAlign, bool &needsHeightAlign) {
    switch(op) {
        case DihedralOp::None:
            needsWidthAlign = false; needsHeightAlign = false; break;
        case DihedralOp::Mirror:
            needsWidthAlign = true;  needsHeightAlign = false; break;
        case DihedralOp::Flip:
            needsWidthAlign = false; needsHeightAlign = true;  break;
        case DihedralOp::Rotate180:
            needsWidthAlign = true;  needsHeightAlign = true;  break;
        case DihedralOp::Rotate90:
            needsWidthAlign = false; needsHeightAlign = true;  break;
        case DihedralOp::MirrorAndRotate90: // Transverse
            needsWidthAlign = true;  needsHeightAlign = true;  break;
        case DihedralOp::FlipAndRotate90: // Transpose
            needsWidthAlign = false; needsHeightAlign = false; break;
        case DihedralOp::Rotate270:
            needsWidthAlign = true;  needsHeightAlign = false; break;
    }
}

static bool readHeader(const QByteArray &src, int &width, int &height, int &subsamp) {
    tjhandle handle = tjInitDecompress();
    if(!handle)
        return false;
    int colorspace = 0;
    int rc = tjDecompressHeader3(handle, reinterpret_cast<const unsigned char*>(src.constData()),
                                  static_cast<unsigned long>(src.size()), &width, &height, &subsamp, &colorspace);
    tjDestroy(handle);
    return rc == 0 && subsamp >= 0 && subsamp < TJ_NUMSAMP;
}

// Whether the file is a progressive JPEG, found by walking the marker
// segments to the frame header. tjTransform() always writes a baseline
// image unless told otherwise, so without this a progressive file would
// silently come back out as baseline - same pixels, but a different
// encoding and a noticeably different file size. (The libjpeg-turbo 2.x
// C API has no way to ask, and tj3Get(TJPARAM_PROGRESSIVE) only exists
// in 3.x.)
static bool isProgressive(const QByteArray &src) {
    const uchar *p = reinterpret_cast<const uchar*>(src.constData());
    int size = src.size();
    if(size < 4 || p[0] != 0xFF || p[1] != 0xD8) // SOI
        return false;
    int i = 2;
    while(i + 3 < size) {
        if(p[i] != 0xFF)
            return false; // not where a marker should be, give up
        uchar marker = p[i + 1];
        if(marker == 0xFF) { // fill byte, skip
            i++;
            continue;
        }
        if(marker == 0xD8 || marker == 0x01 || (marker >= 0xD0 && marker <= 0xD7)) {
            i += 2; // markers without a payload
            continue;
        }
        if(marker == 0xDA || marker == 0xD9) // SOS / EOI: no frame header found
            return false;
        // SOFn: C0-CF except DHT (C4), JPG (C8) and DAC (CC).
        // Progressive DCT is C2 (huffman) or CA (arithmetic).
        if(marker >= 0xC0 && marker <= 0xCF && marker != 0xC4 && marker != 0xC8 && marker != 0xCC)
            return marker == 0xC2 || marker == 0xCA;
        int segmentLength = (int(p[i + 2]) << 8) | int(p[i + 3]);
        if(segmentLength < 2)
            return false;
        i += 2 + segmentLength;
    }
    return false;
}

// The transform copies the metadata markers over untouched, so the parts
// of them that describe the image as it was still need fixing up.
static void fixUpMetadata(QByteArray &jpegBytes) {
    int width, height, subsamp;
    if(!readHeader(jpegBytes, width, height, subsamp))
        return;
    ExifMetadata::fixUpInPlace(jpegBytes, QSize(width, height));
}

static bool runSingleTransform(const QByteArray &src, int tjOp, bool withCrop,
                                int cx, int cy, int cw, int ch, bool progressive, QByteArray &out) {
    tjhandle handle = tjInitTransform();
    if(!handle)
        return false;
    tjtransform xform;
    memset(&xform, 0, sizeof(xform));
    xform.op = tjOp;
    xform.options = TJXOPT_PERFECT;
    if(progressive)
        xform.options |= TJXOPT_PROGRESSIVE;
    if(withCrop) {
        xform.options |= TJXOPT_CROP;
        xform.r.x = cx;
        xform.r.y = cy;
        xform.r.w = cw;
        xform.r.h = ch;
    }
    unsigned char *dstBuf = nullptr;
    unsigned long dstSize = 0;
    int rc = tjTransform(handle, reinterpret_cast<const unsigned char*>(src.constData()),
                          static_cast<unsigned long>(src.size()), 1, &dstBuf, &dstSize, &xform, 0);
    tjDestroy(handle);
    if(rc != 0) {
        if(dstBuf)
            tjFree(dstBuf);
        return false;
    }
    out = QByteArray(reinterpret_cast<const char*>(dstBuf), static_cast<int>(dstSize));
    tjFree(dstBuf);
    return true;
}

// Applies an optional crop followed by an optional geometric op, as two
// separate lossless passes when both are needed: tjTransform() can't
// combine an arbitrary crop region with a rotate/flip op in a single
// "perfect" transform (verified empirically against libjpeg-turbo
// 2.1.5 - a combined call fails with "Transform is not perfect" even
// when the crop region is itself MCU-aligned and would make the op
// perfect on its own).
static bool runCropThenOp(const QByteArray &src, DihedralOp op, std::optional<QRect> region, QByteArray &out) {
    bool progressive = isProgressive(src);
    const QByteArray *current = &src;
    QByteArray cropped;
    if(region) {
        if(!runSingleTransform(src, TJXOP_NONE, true, region->x(), region->y(),
                                region->width(), region->height(), progressive, cropped))
            return false;
        current = &cropped;
    }
    if(op == DihedralOp::None) {
        out = *current;
        return true;
    }
    return runSingleTransform(*current, toTJXOP(op), false, 0, 0, 0, 0, progressive, out);
}

// The other way round: the op on the whole image first, then the crop in
// the transformed image's coordinates. Same result as runCropThenOp()
// with the rectangle mapped through the op, but a different alignment
// requirement - see tryTransform().
static bool runOpThenCrop(const QByteArray &src, DihedralOp op, QRect region, QByteArray &out) {
    bool progressive = isProgressive(src);
    const QByteArray *current = &src;
    QByteArray transformed;
    if(op != DihedralOp::None) {
        if(!runSingleTransform(src, toTJXOP(op), false, 0, 0, 0, 0, progressive, transformed))
            return false;
        current = &transformed;
    }
    return runSingleTransform(*current, TJXOP_NONE, true, region.x(), region.y(),
                               region.width(), region.height(), progressive, out);
}

QSize mcuSize(const QString &filePath) {
    QFile f(filePath);
    if(!f.open(QIODevice::ReadOnly))
        return QSize();
    // The SOF marker sits near the start, but how near depends on how
    // much metadata (EXIF with an embedded thumbnail, ICC profile, XMP)
    // comes first, so read a generous chunk and only fall back to
    // slurping the whole file if that wasn't enough. Matters because
    // this runs per displayed image and photos can be tens of MB.
    QByteArray src = f.read(256 * 1024);
    int width, height, subsamp;
    if(!readHeader(src, width, height, subsamp)) {
        f.seek(0);
        src = f.readAll();
        if(!readHeader(src, width, height, subsamp))
            return QSize();
    }
    return QSize(tjMCUWidth[subsamp], tjMCUHeight[subsamp]);
}

Result tryTransform(const QString &filePath, DihedralOp op, std::optional<QRect> cropRegion, QByteArray &outJpegBytes) {
    QFile f(filePath);
    if(!f.open(QIODevice::ReadOnly))
        return Result::Failed;
    QByteArray src = f.readAll();
    f.close();
    if(src.isEmpty())
        return Result::Failed;

    int width, height, subsamp;
    if(!readHeader(src, width, height, subsamp))
        return Result::Failed;
    int mcuW = tjMCUWidth[subsamp];
    int mcuH = tjMCUHeight[subsamp];

    bool needsWidthAlign, needsHeightAlign;
    requiredAlignment(op, needsWidthAlign, needsHeightAlign);
    bool opFitsWholeImage = !(needsWidthAlign && width % mcuW)
                            && !(needsHeightAlign && height % mcuH);

    QByteArray result;
    if(!cropRegion) {
        if(!opFitsWholeImage)
            return Result::NotAligned;
        if(!runCropThenOp(src, op, std::nullopt, result))
            return Result::Failed;
    } else {
        QRect c = *cropRegion;
        // A crop and an op can be applied in either order (with the
        // rectangle mapped accordingly) for the same result, but the two
        // orders need different things to line up with the MCU grid, so
        // try both before giving up.
        //
        // Crop first: the rectangle's origin has to sit on the file's
        // grid, and the cropped size has to suit the op.
        bool cropFirstFits = c.x() % mcuW == 0 && c.y() % mcuH == 0
                             && !(needsWidthAlign && c.width() % mcuW)
                             && !(needsHeightAlign && c.height() % mcuH);
        // Op first: the op has to suit the whole image, and the
        // rectangle's origin has to sit on the grid of the transformed
        // image (transposed along with it). This is the order that fits
        // how the selection was made - the user drew it on the already
        // rotated view, snapped to that view's grid - so it's what makes
        // "rotate, then crop" work losslessly.
        QRect cPost = mapRect(c, op, QSize(width, height));
        int postMcuW = swapsAxes(op) ? mcuH : mcuW;
        int postMcuH = swapsAxes(op) ? mcuW : mcuH;
        bool opFirstFits = opFitsWholeImage
                           && cPost.x() % postMcuW == 0 && cPost.y() % postMcuH == 0;

        if(cropFirstFits) {
            if(!runCropThenOp(src, op, c, result))
                return Result::Failed;
        } else if(opFirstFits) {
            if(!runOpThenCrop(src, op, cPost, result))
                return Result::Failed;
        } else {
            return Result::NotAligned;
        }
    }

    outJpegBytes = result;
    fixUpMetadata(outJpegBytes);
    return Result::Ok;
}

QByteArray transformWithAlignmentTrim(const QString &filePath, DihedralOp op, std::optional<QRect> requestedCrop) {
    QFile f(filePath);
    if(!f.open(QIODevice::ReadOnly))
        return QByteArray();
    QByteArray src = f.readAll();
    f.close();
    if(src.isEmpty())
        return QByteArray();

    int width, height, subsamp;
    if(!readHeader(src, width, height, subsamp))
        return QByteArray();
    int mcuW = tjMCUWidth[subsamp];
    int mcuH = tjMCUHeight[subsamp];

    bool needsWidthAlign, needsHeightAlign;
    requiredAlignment(op, needsWidthAlign, needsHeightAlign);
    bool opFitsWholeImage = !(needsWidthAlign && width % mcuW)
                            && !(needsHeightAlign && height % mcuH);

    // When the op already suits the whole image, the only thing in the
    // way is where the selection starts, so nudge just that onto the
    // grid of the transformed image and leave everything else alone -
    // much less to give up than trimming the selection's size below.
    if(requestedCrop && opFitsWholeImage) {
        QRect cPost = mapRect(*requestedCrop, op, QSize(width, height));
        int postMcuW = swapsAxes(op) ? mcuH : mcuW;
        int postMcuH = swapsAxes(op) ? mcuW : mcuH;
        int alignedX = ((cPost.x() + postMcuW - 1) / postMcuW) * postMcuW;
        int alignedY = ((cPost.y() + postMcuH - 1) / postMcuH) * postMcuH;
        QRect nudged(alignedX, alignedY,
                      cPost.width() - (alignedX - cPost.x()),
                      cPost.height() - (alignedY - cPost.y()));
        QByteArray result;
        if(nudged.width() > 0 && nudged.height() > 0 && runOpThenCrop(src, op, nudged, result)) {
            fixUpMetadata(result);
            return result;
        }
    }

    int regionX = 0, regionY = 0, regionW = width, regionH = height;
    if(requestedCrop) {
        regionX = requestedCrop->x();
        regionY = requestedCrop->y();
        regionW = requestedCrop->width();
        regionH = requestedCrop->height();
        // move the origin inward, onto the next grid line, so the result
        // stays within what was asked for - the point of this path is to
        // trim a few pixels away, never to add any back
        int alignedX = ((regionX + mcuW - 1) / mcuW) * mcuW;
        int alignedY = ((regionY + mcuH - 1) / mcuH) * mcuH;
        regionW -= alignedX - regionX;
        regionH -= alignedY - regionY;
        regionX = alignedX;
        regionY = alignedY;
    }
    // whatever's left over goes off the right/bottom edge, the only edge
    // that can be shortened by any amount at all - see this function's
    // doc comment
    if(needsWidthAlign)
        regionW -= regionW % mcuW;
    if(needsHeightAlign)
        regionH -= regionH % mcuH;
    if(regionW <= 0 || regionH <= 0)
        return QByteArray();

    std::optional<QRect> region;
    if(regionX != 0 || regionY != 0 || regionW != width || regionH != height)
        region = QRect(regionX, regionY, regionW, regionH);

    QByteArray result;
    if(!runCropThenOp(src, op, region, result))
        return QByteArray();

    fixUpMetadata(result);
    return result;
}

} // namespace LosslessJpegTransform

#endif // USE_TURBOJPEG
