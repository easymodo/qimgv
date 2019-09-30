#include "documentinfo.h"

DocumentInfo::DocumentInfo(QString path)
    : mImageType(NONE),
      mOrientation(0),
      mExtension("")
{
    fileInfo.setFile(path);
    if(!fileInfo.isFile()) {
        qDebug() << "FileInfo: cannot open: " << path;
        return;
    }
    detectType();
    loadExifInfo();
}

DocumentInfo::~DocumentInfo() {
}

// ##############################################################
// ####################### PUBLIC METHODS #######################
// ##############################################################

QString DocumentInfo::directoryPath() const {
    return fileInfo.absolutePath();
}

QString DocumentInfo::filePath() const {
    return fileInfo.absoluteFilePath();
}

QString DocumentInfo::fileName() const {
    return fileInfo.fileName();
}

QString DocumentInfo::baseName() const {
    return fileInfo.baseName();
}

// bytes
int DocumentInfo::fileSize() const {
    return static_cast<int>(fileInfo.size());
}

DocumentType DocumentInfo::type() const {
    return mImageType;
}

QMimeType DocumentInfo::mimeType() const {
    return mMimeType;
}
const char *DocumentInfo::extension() const {
    return mExtension.toStdString().c_str();
}

QDateTime DocumentInfo::lastModified() const {
    return fileInfo.lastModified();
}

// For cases like orientation / even mimetype change we just reload
// Image from scratch, so don`t bother handling it here
void DocumentInfo::refresh() {
    fileInfo.refresh();
}

int DocumentInfo::exifOrientation() const {
    return mOrientation;
}

// ##############################################################
// ####################### PRIVATE METHODS ######################
// ##############################################################

// detect correct file extension
// TODO: this is just bad
void DocumentInfo::detectType() {
    QMimeDatabase mimeDb;
    mMimeType = mimeDb.mimeTypeForFile(fileInfo.filePath(), QMimeDatabase::MatchContent);
    QString mimeName = mMimeType.name();
    QString suffix = fileInfo.completeSuffix();
    // todo: split mime-based detection vs exension-based
    if(mimeName == "application/octet-stream") {
        if(QString::compare(fileInfo.completeSuffix(), "webm", Qt::CaseInsensitive) == 0) {
            mExtension = "webm";
            mImageType = DocumentType::VIDEO;
        }
        if(QString::compare(fileInfo.completeSuffix(), "mp4", Qt::CaseInsensitive) == 0) {
            mExtension = "mp4";
            mImageType = DocumentType::VIDEO;
        }
    } else if(mimeName == "video/webm") {
        mExtension = "webm";
        mImageType = DocumentType::VIDEO;
    } else if(mimeName == "video/mp4") {
        mExtension = "mp4";
        mImageType = DocumentType::VIDEO;
    } else if(mimeName == "image/jpeg") {
        mExtension = "jpg";
        mImageType = STATIC;
    } else if(mimeName == "image/png") {
        if(QImageReader::supportedImageFormats().contains("apng") && detectAPNG()) {
            mExtension = "apng";
            mImageType = ANIMATED;
        } else {
            mExtension = "png";
            mImageType = STATIC;
        }
    } else if(mimeName == "image/gif") {
        mExtension = "gif";
        mImageType = ANIMATED;
    // webp is incorrectly(?) detected as audio/x-riff on my windows pc
    } else if(mimeName == "audio/x-riff") {
        // in case we encounter an actual audio/x-riff
        if(QString::compare(fileInfo.completeSuffix(), "webp", Qt::CaseInsensitive) == 0) {
            mExtension = "webp";
            mImageType = detectAnimatedWebP() ? ANIMATED : STATIC;
        }
    // TODO: parse header to find out if it supports animation.
    // treat all webp as animated for now.
    } else if(mimeName == "image/webp") {
        mExtension = "webp";
        mImageType = detectAnimatedWebP() ? ANIMATED : STATIC;
    } else if(mimeName == "image/bmp") {
        mExtension = "bmp";
        mImageType = STATIC;
    } else {
        mExtension = fileInfo.completeSuffix();
        mImageType = STATIC;
    }
}

inline
// dumb apng detector
bool DocumentInfo::detectAPNG() {
    QFile f(fileInfo.filePath());
    if(f.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&f);
        QString header(in.read(120)); // 120 chars should be sufficient?
        return header.contains("acTL");
    }
    return false;
}

bool DocumentInfo::detectAnimatedWebP() {
    QFile f(fileInfo.filePath());
    bool result = false;
    if(f.open(QFile::ReadOnly)) {
        QDataStream in(&f);
        in.skipRawData(12);
        char *buf = static_cast<char*>(malloc(5));
        buf[4] = '\0';
        in.readRawData(buf, 4);
        if(strcmp(buf, "VP8X") == 0) {
            in.skipRawData(4);
            char flags;
            in.readRawData(&flags, 1);
            if(flags & (1 << 1)) {
                result = true;
            }
        }
        free(buf);
    }
    return result;
}

void DocumentInfo::loadExifInfo() {
    exifTags.clear();
    loadExifOrientation();
#ifdef USE_EXIV2
    try {
        std::unique_ptr<Exiv2::Image> image;

        image = Exiv2::ImageFactory::open(toStdString(fileInfo.filePath()));

        assert(image.get() != 0);
        image->readMetadata();
        Exiv2::ExifData &exifData = image->exifData();
        if(exifData.empty())
            return;

        Exiv2::ExifKey make("Exif.Image.Make");
        Exiv2::ExifKey model("Exif.Image.Model");
        Exiv2::ExifKey dateTime("Exif.Image.DateTime");
        Exiv2::ExifKey exposureTime("Exif.Photo.ExposureTime");
        Exiv2::ExifKey fnumber("Exif.Photo.FNumber");
        Exiv2::ExifKey isoSpeedRatings("Exif.Photo.ISOSpeedRatings");
        Exiv2::ExifKey flash("Exif.Photo.Flash");
        Exiv2::ExifKey focalLength("Exif.Photo.FocalLength");

        Exiv2::ExifData::const_iterator it;

        it = exifData.findKey(make);
        if(it != exifData.end() /* && it->count() */)
            exifTags.insert("Make", QString::fromStdString(it->value().toString()));

        it = exifData.findKey(model);
        if(it != exifData.end())
            exifTags.insert("Model", QString::fromStdString(it->value().toString()));

        it = exifData.findKey(dateTime);
        if(it != exifData.end())
            exifTags.insert("Date/Time", QString::fromStdString(it->value().toString()));

        it = exifData.findKey(exposureTime);
        if(it != exifData.end()) {
            Exiv2::Rational r = it->toRational();
            if(r.first < r.second) {
                qreal exp = round(static_cast<qreal>(r.second) / r.first);
                exifTags.insert("ExposureTime", "1/" + QString::number(exp) + " sec");
            } else {
                qreal exp = round(static_cast<qreal>(r.first) / r.second);
                exifTags.insert("ExposureTime", QString::number(exp) + " sec");
            }
        }

        it = exifData.findKey(fnumber);
        if(it != exifData.end()) {
            Exiv2::Rational r = it->toRational();
            qreal fn = static_cast<qreal>(r.first) / r.second;
            exifTags.insert("F Number", "f/" + QString::number(fn, 'g', 3));
        }

        it = exifData.findKey(isoSpeedRatings);
        if(it != exifData.end())
            exifTags.insert("ISO Speed ratings", QString::fromStdString(it->value().toString()));

        it = exifData.findKey(flash);
        if(it != exifData.end())
            exifTags.insert("Flash", QString::fromStdString(it->value().toString()));

        it = exifData.findKey(focalLength);
        if(it != exifData.end()) {
            Exiv2::Rational r = it->toRational();
            qreal fn = static_cast<qreal>(r.first) / r.second;
            exifTags.insert("Focal Length", QString::number(fn, 'g', 3) + " mm");
        }

        return;
    }
    catch (Exiv2::Error& e) {
        std::cout << "Caught Exiv2 exception '" << e.what() << "'\n";
        return;
    }
#endif
}

QMap<QString, QString> DocumentInfo::getExifTags() {
    return exifTags;
}

void DocumentInfo::loadExifOrientation() {
    if(mImageType == VIDEO || mImageType == NONE)
        return;

    QString path = filePath();
    QImageReader *reader = nullptr;
    if(!mExtension.isEmpty())
        reader = new QImageReader(path, mExtension.toStdString().c_str());
    else
        reader = new QImageReader(path);

    if(reader->canRead())
        mOrientation = static_cast<int>(reader->transformation());
    delete reader;
}
