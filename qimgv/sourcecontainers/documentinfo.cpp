#include "documentinfo.h"

DocumentInfo::DocumentInfo(QString path)
    : mDocumentType(DocumentType::NONE),
      mOrientation(0),
      mFormat(""),
      exifLoaded(false)
{
    fileInfo.setFile(path);
    if(!fileInfo.isFile()) {
        qDebug() << "FileInfo: cannot open: " << path;
        return;
    }
    detectFormat();
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
qint64 DocumentInfo::fileSize() const {
    return fileInfo.size();
}

DocumentType DocumentInfo::type() const {
    return mDocumentType;
}

QMimeType DocumentInfo::mimeType() const {
    return mMimeType;
}

QString DocumentInfo::format() const {
    return mFormat;
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
void DocumentInfo::detectFormat() {
    if(mDocumentType != DocumentType::NONE)
        return;
    QMimeDatabase mimeDb;
    mMimeType = mimeDb.mimeTypeForFile(fileInfo.filePath(), QMimeDatabase::MatchContent);
    auto mimeName = mMimeType.name().toUtf8();
    auto suffix = fileInfo.suffix().toLower().toUtf8();
    if(mimeName == "image/jpeg") {
        mFormat = "jpg";
        mDocumentType = DocumentType::STATIC;
    } else if(mimeName == "image/png") {
        if(QImageReader::supportedImageFormats().contains("apng") && detectAPNG()) {
            mFormat = "apng";
            mDocumentType = DocumentType::ANIMATED;
        } else {
            mFormat = "png";
            mDocumentType = DocumentType::STATIC;
        }
    } else if(mimeName == "image/gif") {
        mFormat = "gif";
        mDocumentType = DocumentType::ANIMATED;
    } else if(mimeName == "image/webp" || (mimeName == "audio/x-riff" && suffix == "webp")) {
        mFormat = "webp";
        mDocumentType = detectAnimatedWebP() ? DocumentType::ANIMATED : DocumentType::STATIC;
    } else if(mimeName == "image/jxl") {
        mFormat = "jxl";
        mDocumentType = detectAnimatedJxl() ? DocumentType::ANIMATED : DocumentType::STATIC;
        if(mDocumentType == DocumentType::ANIMATED && !settings->jxlAnimation()) {
            mDocumentType = DocumentType::NONE;
            qDebug() << "animated jxl is off; skipping file";
        }
    } else if(mimeName == "image/avif") {
        mFormat = "avif";
        mDocumentType = detectAnimatedAvif() ? DocumentType::ANIMATED : DocumentType::STATIC;
    } else if(mimeName == "image/bmp") {
        mFormat = "bmp";
        mDocumentType = DocumentType::STATIC;
    } else if(settings->videoPlayback() && settings->videoFormats().contains(mimeName)) {
        mDocumentType = DocumentType::VIDEO;
        mFormat = settings->videoFormats().value(mimeName);
    } else {
        // just try to open via suffix if all of the above fails
        mFormat = suffix;
        if(mFormat.compare("jfif", Qt::CaseInsensitive) == 0)
            mFormat = "jpg";
        if(settings->videoPlayback() && settings->videoFormats().values().contains(suffix))
            mDocumentType = DocumentType::VIDEO;
        else
            mDocumentType = DocumentType::STATIC;
    }
    loadExifOrientation();
}

inline
// dumb apng detector
bool DocumentInfo::detectAPNG() {
    QFile f(fileInfo.filePath());
    if(f.open(QFile::ReadOnly)) {
        QDataStream in(&f);
        const int len = 120;
        QByteArray qbuf("\0", len);
        if (in.readRawData(qbuf.data(), len) > 0) {
            return qbuf.contains("acTL");
        }
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

// TODO avoid creating multiple QImageReader instances
bool DocumentInfo::detectAnimatedJxl() {
    QImageReader r(fileInfo.filePath(), "jxl");
    return r.supportsAnimation();
}

bool DocumentInfo::detectAnimatedAvif() {
    QFile f(fileInfo.filePath());
    bool result = false;
    if(f.open(QFile::ReadOnly)) {
        QDataStream in(&f);
        in.skipRawData(4); // skip box size
        char *buf = static_cast<char*>(malloc(9));
        buf[8] = '\0';
        in.readRawData(buf, 8);
        if(strcmp(buf, "ftypavis") == 0) {
            result = true;
        }
        free(buf);
    }
    return result;
}

// tries a list of candidate exif keys in order (maker notes vary by vendor
// and by exiv2 version), returns the first one both known to this exiv2
// build and present in the file
static Exiv2::ExifData::const_iterator findFirstExifKey(const Exiv2::ExifData &exifData,
                                                         const std::initializer_list<const char *> &candidates)
{
    for(const char *keyName : candidates) {
        try {
            auto it = exifData.findKey(Exiv2::ExifKey(keyName));
            if(it != exifData.end())
                return it;
        } catch(const Exiv2::Error &) {
            // unknown group/tag for this exiv2 build - try the next candidate
        }
    }
    return exifData.end();
}

void DocumentInfo::loadExifTags() {
    if(exifLoaded)
        return;
    exifLoaded = true;
    exifTags.clear();
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
        Exiv2::ExifKey dateTimeOriginal("Exif.Photo.DateTimeOriginal");
        Exiv2::ExifKey dateTimeDigitized("Exif.Photo.DateTimeDigitized");
        Exiv2::ExifKey dateTime("Exif.Image.DateTime");
        Exiv2::ExifKey exposureTime("Exif.Photo.ExposureTime");
        Exiv2::ExifKey fnumber("Exif.Photo.FNumber");
        Exiv2::ExifKey exposureBias("Exif.Photo.ExposureBiasValue");
        Exiv2::ExifKey isoSpeedRatings("Exif.Photo.ISOSpeedRatings");
        Exiv2::ExifKey flash("Exif.Photo.Flash");
        Exiv2::ExifKey focalLength("Exif.Photo.FocalLength");
        Exiv2::ExifKey lensSpecification("Exif.Photo.LensSpecification");
        Exiv2::ExifKey userComment("Exif.Photo.UserComment");
        Exiv2::ExifKey orientation("Exif.Image.Orientation");
        Exiv2::ExifKey software("Exif.Image.Software");
        Exiv2::ExifKey gpsLatitude("Exif.GPSInfo.GPSLatitude");
        Exiv2::ExifKey gpsLongitude("Exif.GPSInfo.GPSLongitude");

        Exiv2::ExifData::const_iterator it;

        it = exifData.findKey(make);
        if(it != exifData.end() /* && it->count() */)
            exifTags.insert("Make", QString::fromStdString(it->value().toString()));

        it = exifData.findKey(model);
        if(it != exifData.end())
            exifTags.insert("Model", QString::fromStdString(it->value().toString()));

        // prefer the tags that record when the shot was actually taken;
        // Exif.Image.DateTime is only "last file change" per spec and may
        // have been overwritten by an editing tool, so it's tried last
        it = exifData.findKey(dateTimeOriginal);
        if(it == exifData.end())
            it = exifData.findKey(dateTimeDigitized);
        if(it == exifData.end())
            it = exifData.findKey(dateTime);
        if(it != exifData.end())
            exifTags.insert("DateTime", QString::fromStdString(it->value().toString()));

        it = exifData.findKey(exposureTime);
        bool haveExposureTime = it != exifData.end();
        qreal exposureTimeSec = 0;
        if(haveExposureTime) {
            Exiv2::Rational r = it->toRational();
            exposureTimeSec = r.second ? static_cast<qreal>(r.first) / r.second : 0;
            if(r.first < r.second) {
                qreal exp = round(static_cast<qreal>(r.second) / r.first);
                exifTags.insert("ExposureTime", "1/" + QString::number(exp) + QObject::tr(" sec"));
            } else {
                qreal exp = round(static_cast<qreal>(r.first) / r.second);
                exifTags.insert("ExposureTime", QString::number(exp) + QObject::tr(" sec"));
            }
        }

        it = exifData.findKey(fnumber);
        bool haveFNumber = it != exifData.end();
        qreal fNumberValue = 0;
        if(haveFNumber) {
            Exiv2::Rational r = it->toRational();
            fNumberValue = r.second ? static_cast<qreal>(r.first) / r.second : 0;
            exifTags.insert("FNumber", "f/" + QString::number(fNumberValue, 'g', 3));
        }

        it = exifData.findKey(exposureBias);
        if(it != exifData.end()) {
            Exiv2::Rational r = it->toRational();
            if(r.second) {
                qreal ev = static_cast<qreal>(r.first) / r.second;
                exifTags.insert("ExposureCompensation",
                                 (ev > 0 ? "+" : "") + QString::number(ev, 'f', 1) + " EV");
            }
        }

        it = exifData.findKey(isoSpeedRatings);
        bool haveISO = it != exifData.end();
        qreal isoValue = 0;
        if(haveISO) {
            isoValue = it->toFloat();
            exifTags.insert("ISOSpeedRatings", QString::fromStdString(it->value().toString()));
        }

        // Composite exposure/"light" value, same formula as exiftool's
        // Composite:LightValue - LV = log2(N^2 * 100 / (t * ISO))
        if(haveFNumber && haveExposureTime && haveISO && fNumberValue > 0 && exposureTimeSec > 0 && isoValue > 0) {
            qreal lv = std::log2((fNumberValue * fNumberValue * 100.0) / (exposureTimeSec * isoValue));
            exifTags.insert("LightValue", QString::number(lv, 'f', 1));
        }

        it = exifData.findKey(flash);
        if(it != exifData.end())
            exifTags.insert("Flash", QString::fromStdString(it->print(&exifData)));

        it = exifData.findKey(focalLength);
        if(it != exifData.end()) {
            Exiv2::Rational r = it->toRational();
            qreal fn = static_cast<qreal>(r.first) / r.second;
            exifTags.insert("FocalLength", QString::number(fn, 'g', 3) + QObject::tr(" mm"));
        }

        it = exifData.findKey(lensSpecification);
        if(it != exifData.end() && it->count() >= 2) {
            qreal minFocal = it->toFloat(0);
            qreal maxFocal = it->toFloat(1);
            if(minFocal > 0 && maxFocal > 0) {
                if(qFuzzyCompare(minFocal, maxFocal))
                    exifTags.insert("LensRange", QString::number(minFocal, 'g', 3) + QObject::tr(" mm"));
                else
                    exifTags.insert("LensRange", QString::number(minFocal, 'g', 3) + " - " +
                                                  QString::number(maxFocal, 'g', 3) + QObject::tr(" mm"));
            }
        }

        it = exifData.findKey(userComment);
        if(it != exifData.end()) {
            // crop out 'charset=ascii' etc"
            auto comment = QString::fromStdString(it->value().toString());
            if(comment.startsWith("charset="))
                comment.remove(0, comment.indexOf(" ") + 1);
            exifTags.insert("UserComment", comment);
        }

        it = exifData.findKey(orientation);
        if(it != exifData.end()) {
            // exiv2's own print() spells this out as e.g. "right, top" (which
            // edge of the original data is the top/left of the scene) - a
            // literal but unintuitive reading of the spec. The tag only has
            // 8 defined values, so a small custom table reads much better.
            static const QMap<long, QString> orientationTable = {
                {1, QObject::tr("Normal")},
                {2, QObject::tr("Mirrored horizontal")},
                {3, QObject::tr("Rotated 180°")},
                {4, QObject::tr("Mirrored vertical")},
                {5, QObject::tr("Mirrored horizontal, rotated 90° CCW")},
                {6, QObject::tr("Rotated 90° CW")},
                {7, QObject::tr("Mirrored horizontal, rotated 90° CW")},
                {8, QObject::tr("Rotated 90° CCW")},
            };
            auto tableIt = orientationTable.constFind(it->toLong());
            if(tableIt != orientationTable.constEnd())
                exifTags.insert("Orientation", tableIt.value());
            else
                exifTags.insert("Orientation", QString::fromStdString(it->print(&exifData)));
        }

        it = exifData.findKey(software);
        if(it != exifData.end())
            exifTags.insert("Software", QString::fromStdString(it->value().toString()));

        // GPS position: no single dedicated tag, so it's built by joining
        // the printed DMS lat & lon values; exiv2's print() spells the
        // degree sign out as "deg" and (for these tags) doesn't include
        // the hemisphere, so both are fixed up using the Ref tags
        auto latIt = exifData.findKey(gpsLatitude);
        auto lonIt = exifData.findKey(gpsLongitude);
        if(latIt != exifData.end() && lonIt != exifData.end()) {
            QString lat = QString::fromStdString(latIt->print(&exifData)).replace("deg", QChar(0xB0));
            QString lon = QString::fromStdString(lonIt->print(&exifData)).replace("deg", QChar(0xB0));

            auto latRefIt = exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLatitudeRef"));
            auto lonRefIt = exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLongitudeRef"));
            if(latRefIt != exifData.end())
                lat += " " + QString::fromStdString(latRefIt->value().toString());
            if(lonRefIt != exifData.end())
                lon += " " + QString::fromStdString(lonRefIt->value().toString());

            exifTags.insert("GPSPosition", lat + ", " + lon);
        }

        // Focus Mode & Lens ID: not part of the standard Exif.Photo.*
        // namespace, only ever present in vendor-specific maker notes.
        // Best-effort lookup, prioritizing Sony, with common alternatives
        // as a fallback; safe to fail silently on unsupported cameras or
        // an exiv2 build without a given maker note's tag tables.
        it = findFirstExifKey(exifData, {
            "Exif.Sony1.FocusMode",
            "Exif.Sony2.FocusMode",
            "Exif.CanonCs.FocusMode",
            "Exif.Nikon3.FocusMode",
            "Exif.Panasonic.FocusMode",
            "Exif.Pentax.FocusMode",
        });
        if(it != exifData.end())
            exifTags.insert("FocusMode", QString::fromStdString(it->print(&exifData)));

        it = findFirstExifKey(exifData, {
            "Exif.Sony1.LensID",
            "Exif.Sony2.LensID",
            "Exif.Photo.LensModel",
            "Exif.CanonCs.LensType",
            "Exif.Canon.LensModel",
            "Exif.NikonLd3.LensIDNumber",
        });
        if(it != exifData.end())
            exifTags.insert("LensID", QString::fromStdString(it->print(&exifData)));
    }

// this should work with both 0.28 and <0.28
#if not EXIV2_TEST_VERSION(0, 28, 0)
#ifdef __WIN32
    catch (Exiv2::BasicError<wchar_t>& e) {
        qDebug() << "Caught Exiv2::BasicError exception:\n" << e.what() << "\n";
        return;
    }
#else
    catch (Exiv2::BasicError<char>& e) {
        qDebug() << "Caught Exiv2::BasicError exception:\n" << e.what() << "\n";
        return;
    }
#endif
#endif

    catch (Exiv2::Error& e) {
        qDebug() << "Caught Exiv2 exception:\n" << e.what() << "\n";
        return;
    }
#endif
}

QStringList DocumentInfo::exifFieldKeys() {
    static const QStringList keys = {
        "Make", "Model", "LensID", "DateTime", "ExposureTime", "FNumber",
        "ExposureCompensation", "ISOSpeedRatings", "LightValue", "FocalLength",
        "LensRange", "FocusMode", "Flash", "Orientation", "GPSPosition",
        "Software", "UserComment"
    };
    return keys;
}

QString DocumentInfo::exifFieldLabel(const QString &key) {
    if(key == "Make")                  return QObject::tr("Make");
    if(key == "Model")                 return QObject::tr("Model");
    if(key == "LensID")                return QObject::tr("Lens ID");
    if(key == "DateTime")              return QObject::tr("Date/Time");
    if(key == "ExposureTime")          return QObject::tr("ExposureTime");
    if(key == "FNumber")               return QObject::tr("F Number");
    if(key == "ExposureCompensation")  return QObject::tr("Exposure Compensation");
    if(key == "ISOSpeedRatings")       return QObject::tr("ISO Speed ratings");
    if(key == "LightValue")            return QObject::tr("Light Value");
    if(key == "FocalLength")           return QObject::tr("Focal Length");
    if(key == "LensRange")             return QObject::tr("Lens Range");
    if(key == "FocusMode")             return QObject::tr("Focus Mode");
    if(key == "Flash")                 return QObject::tr("Flash");
    if(key == "Orientation")           return QObject::tr("Orientation");
    if(key == "GPSPosition")           return QObject::tr("GPS Position");
    if(key == "Software")              return QObject::tr("Software");
    if(key == "UserComment")           return QObject::tr("UserComment");
    return key;
}

QVector<QPair<QString, QString>> DocumentInfo::getExifTags() {
    if(!exifLoaded)
        loadExifTags();
    QVector<QPair<QString, QString>> result;
    const auto fields = settings->exifFields();
    for(const auto &field : fields) {
        if(!field.second)
            continue;
        auto it = exifTags.constFind(field.first);
        if(it != exifTags.constEnd())
            result.append({exifFieldLabel(field.first), it.value()});
    }
    return result;
}

void DocumentInfo::loadExifOrientation() {
    if(mDocumentType == DocumentType::VIDEO || mDocumentType == DocumentType::NONE)
        return;

    QString path = filePath();
    QImageReader *reader = nullptr;
    if(!mFormat.isEmpty())
        reader = new QImageReader(path, mFormat.toStdString().c_str());
    else
        reader = new QImageReader(path);

    if(reader->canRead())
        mOrientation = static_cast<int>(reader->transformation());
    delete reader;
}
