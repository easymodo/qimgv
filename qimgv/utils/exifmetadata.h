#pragma once

#include <QString>
#include <QByteArray>
#include <QSize>
#include <memory>

// Carries a file's EXIF/IPTC/XMP metadata across a save, since Qt drops
// it: QImage doesn't hold on to it, so anything written through
// QImage::save() comes out with the metadata gone.
//
// Editing an image invalidates parts of its own metadata, so both save
// paths (re-encoded and lossless) run the same fixups through here:
// the orientation is now baked into the pixels, the recorded pixel
// dimensions may have changed, and the embedded thumbnail still shows
// the image as it was before the edit.
//
// Everything here is best-effort and a no-op without Exiv2: metadata is
// worth keeping, but never worth failing a save over.
namespace ExifMetadata {

class Bundle {
public:
    Bundle();
    ~Bundle();
    Bundle(Bundle &&other) noexcept;
    Bundle &operator=(Bundle &&other) noexcept;

    bool isEmpty() const;

private:
    friend Bundle read(const QString &filePath);
    friend void writeTo(const Bundle &metadata, const QString &filePath, QSize imageSize);
    struct Data;
    std::unique_ptr<Data> d;
};

// Reads the metadata of `filePath`. Call before overwriting the file.
Bundle read(const QString &filePath);

// Writes `metadata` onto the image at `filePath`, which is expected to
// hold the edited image at `imageSize`.
void writeTo(const Bundle &metadata, const QString &filePath, QSize imageSize);

// Same fixups, on a JPEG that is still in memory, keeping the metadata
// it already carries (a lossless transform copies the markers over as
// they were).
void fixUpInPlace(QByteArray &jpegBytes, QSize imageSize);

} // namespace ExifMetadata
