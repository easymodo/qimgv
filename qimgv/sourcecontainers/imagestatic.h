#pragma once

#include <QImage>
#include <QImageWriter>
#include <QSemaphore>
#include <QCryptographicHash>
#include <optional>
#include "image.h"
#include "utils/imagelib.h"
#include "utils/losslessjpegtransform.h"
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

    // Tracks the rotates/flips/crops performed since the file was loaded
    // or last saved, in a form that can be replayed on the JPEG file
    // itself, so a save can apply them losslessly instead of re-encoding
    // the (already edited, for preview purposes) raster image.
    // See Core::rotateByDegrees()/flipH()/flipV()/crop() and
    // Core::saveFile().
    void addPendingLosslessOp(LosslessJpegTransform::DihedralOp op);
    void addPendingLosslessCrop(QRect rect, QSize sizeBeforeCrop);
    // gives up on lossless tracking: the pending edits can't be
    // expressed as a JPEG transform anymore (e.g. after a resize)
    void invalidatePendingLossless();
    // starts tracking from the file as it is on disk, EXIF orientation
    // and all
    void initPendingLossless();
    // back to "nothing done yet" (the edits were discarded)
    void resetPendingLossless();
    // the displayed pixels were just written to this very file, so
    // there's nothing left to replay onto it
    void markPendingLosslessSaved();
    bool hasPendingLosslessChanges() const;
    LosslessJpegTransform::PendingTransform pendingLossless() const;
    // MCU block size in the coordinates of the currently displayed
    // image, for snapping a crop selection to it. Empty if unavailable.
    QSize losslessMcuSize();

public slots:
    void crop(QRect newRect);
    bool save();
    bool save(QString destPath);
    // Writes already-transformed JPEG bytes (produced by
    // LosslessJpegTransform) directly to disk, bypassing QImage::save().
    // Same backup/rollback behavior as save().
    bool saveLosslessBytes(QString destPath, const QByteArray &jpegBytes);

private:
    void load();
    std::shared_ptr<const QImage> image, imageEdited;
    void loadGeneric();
    void loadICO();
    QString generateHash(QString str);

    std::optional<LosslessJpegTransform::PendingTransform> mPendingLossless;
    // the transform that maps the file on disk to what's displayed when
    // nothing has been edited yet: the file's EXIF orientation, or the
    // identity once we've written the displayed pixels out ourselves
    LosslessJpegTransform::DihedralOp mLosslessBaseOp = LosslessJpegTransform::DihedralOp::None;
    QSize mRawMcuSize;
    bool mRawMcuSizeChecked = false;
};
