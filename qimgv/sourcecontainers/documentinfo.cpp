#include "documentinfo.h"
#include <QFile>
#include <QDataStream>
#include <QtEndian>
#include <QTimeZone>
#include <QRegularExpression>

// Helper function to extract timezone from ISO 8601 datetime string
// Returns empty string if no timezone found
static QString extractTimezone(const QString &dateTimeStr) {
    static const QRegularExpression tzRegex("([+-]\\d{2}:\\d{2}|Z)$");
    QRegularExpressionMatch match = tzRegex.match(dateTimeStr);
    if (match.hasMatch()) {
        QString tz = match.captured(1);
        return (tz == "Z") ? "+00:00" : tz;
    }
    return QString();
}

// Helper function to format timezone offset minutes as +HH:MM or -HH:MM
static QString formatTimezone(qint16 tzMinutes) {
    int hours = tzMinutes / 60;
    int minutes = qAbs(tzMinutes % 60);
    return QString("%1%2:%3")
        .arg(tzMinutes >= 0 ? "+" : "-")
        .arg(qAbs(hours), 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'));
}

// Helper function to parse XMP datetime string and extract both datetime and timezone
// Returns true if datetime was parsed successfully
static bool parseXmpDateTime(const QString &dateTimeStr, QDateTime &outDateTime, QString &outTimezone) {
    // Extract timezone if present and not already found
    if (outTimezone.isEmpty()) {
        QString tz = extractTimezone(dateTimeStr);
        if (!tz.isEmpty()) {
            outTimezone = tz;
        }
    }

    // Remove timezone suffix to parse as LocalTime (timezone-naive)
    QString cleanDateTimeStr = dateTimeStr;
    if (!outTimezone.isEmpty()) {
        // Remove the timezone part (e.g., "+02:00" or "Z")
        cleanDateTimeStr = dateTimeStr.left(dateTimeStr.length() - outTimezone.length());
    }

    // Try ISO 8601 format first (YYYY-MM-DDTHH:MM:SS without timezone)
    outDateTime = QDateTime::fromString(cleanDateTimeStr, Qt::ISODate);

    // Fallback to EXIF format (no timezone)
    if (!outDateTime.isValid()) {
        outDateTime = QDateTime::fromString(cleanDateTimeStr, "yyyy:MM:dd HH:mm:ss");
    }

    // Also try hyphenated format
    if (!outDateTime.isValid()) {
        outDateTime = QDateTime::fromString(cleanDateTimeStr, "yyyy-MM-dd HH:mm:ss");
    }

    // Ensure it's stored as LocalTime (timezone-naive)
    if (outDateTime.isValid() && outDateTime.timeSpec() != Qt::LocalTime) {
        outDateTime = QDateTime(outDateTime.date(), outDateTime.time(), Qt::LocalTime);
    }

    return outDateTime.isValid();
}

// Helper function to read QuickTime creation date and timezone from MP4/MOV files
// Returns the datetime and the timezone string (e.g., "+02:00") via timezoneStr parameter
// timezoneStr will be empty if no timezone atom was found
static QDateTime readQuickTimeCreationDate(const QString &filePath, QString &timezoneStr) {
    timezoneStr.clear();
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return QDateTime();

    qint64 fileSize = file.size();
    QDateTime creationDateTime;

    // Try two strategies: search from beginning, or from end
    QList<qint64> searchPositions;
    searchPositions << 0;  // Start from beginning
    if (fileSize > 500*1024) {
        searchPositions << (fileSize - 500*1024);  // Also try end
    }

    for (qint64 searchStart : searchPositions) {
        file.seek(searchStart);
        QDataStream stream(&file);
        stream.setByteOrder(QDataStream::BigEndian);

        int atomCount = 0;
        while (!stream.atEnd() && atomCount < 50) {
            qint64 atomStart = file.pos();
            quint32 atomSize32;
            char atomType[5] = {0};

            if (stream.readRawData((char*)&atomSize32, 4) != 4) break;
            atomSize32 = qFromBigEndian(atomSize32);
            if (stream.readRawData(atomType, 4) != 4) break;

            qint64 atomSize = atomSize32;

            // Handle extended size (size == 1 means read next 8 bytes for 64-bit size)
            if (atomSize32 == 1) {
                quint64 atomSize64;
                if (stream.readRawData((char*)&atomSize64, 8) != 8) break;
                atomSize = qFromBigEndian(atomSize64);
            }

            if (atomSize < 8 || atomSize > fileSize) {
                break;
            }

            atomCount++;

            // If we found the moov container, search inside it for mvhd and optionally timezone
            if (strcmp(atomType, "moov") == 0) {
                qint64 moovEnd = file.pos() + atomSize - 8;

                while (file.pos() < moovEnd && !stream.atEnd()) {
                    // Early exit if we found both datetime and timezone
                    if (creationDateTime.isValid() && !timezoneStr.isEmpty()) {
                        break;
                    }

                    quint32 subAtomSize;
                    char subAtomType[5] = {0};

                    if (stream.readRawData((char*)&subAtomSize, 4) != 4) break;
                    subAtomSize = qFromBigEndian(subAtomSize);
                    if (stream.readRawData(subAtomType, 4) != 4) break;

                    if (subAtomSize < 8) break;

                    // Read creation time from mvhd
                    if (strcmp(subAtomType, "mvhd") == 0) {
                        quint8 version;
                        if (stream.readRawData((char*)&version, 1) != 1) break;
                        stream.skipRawData(3); // flags

                        quint32 creationTime;
                        if (stream.readRawData((char*)&creationTime, 4) != 4) break;
                        creationTime = qFromBigEndian(creationTime);

                        const quint32 QT_EPOCH_OFFSET = 2082844800;
                        if (creationTime > QT_EPOCH_OFFSET) {
                            qint64 unixTime = creationTime - QT_EPOCH_OFFSET;
                            // QuickTime stores time as timezone-naive (local time at recording)
                            // Create as LocalTime to match EXIF photo timestamps
                            QDateTime utcTime = QDateTime::fromSecsSinceEpoch(unixTime, Qt::UTC);
                            creationDateTime = QDateTime(utcTime.date(), utcTime.time(), Qt::LocalTime);
                        }
                    }
                    // Parse UUID atoms (timezone may be stored here)
                    else if (strcmp(subAtomType, "uuid") == 0 && subAtomSize >= 24) {
                        // UUID atoms have: 16-byte UUID, then data
                        char uuid[16];
                        if (stream.readRawData(uuid, 16) == 16) {
                            // Read the remaining data as potential timezone info
                            qint64 dataSize = subAtomSize - 24; // atom header (8) + uuid (16)
                            if (dataSize >= 2 && dataSize < 1000) {
                                QByteArray uuidData(dataSize, 0);
                                if (stream.readRawData(uuidData.data(), dataSize) == dataSize) {
                                    // Timezone stored as signed 16-bit minutes near the end
                                    qint16 tzMinutes = qFromBigEndian<qint16>((const uchar*)&uuidData.data()[dataSize - 2]);
                                    // Valid timezone range: -720 to +840 minutes (-12:00 to +14:00)
                                    // Exclude 0 because files without timezone often have 0 in padding bytes
                                    if (tzMinutes != 0 && tzMinutes >= -720 && tzMinutes <= 840) {
                                        timezoneStr = formatTimezone(tzMinutes);
                                    }
                                }
                            }
                        }
                        // UUID atoms are already positioned correctly by the read
                        continue;
                    }
                    // Look for metadata container atoms
                    else if (strcmp(subAtomType, "SDLN") == 0 || strcmp(subAtomType, "SOID") == 0) {
                        // Metadata container - search for timezone inside
                        qint64 containerEnd = file.pos() + subAtomSize - 8;
                        while (file.pos() < containerEnd && !stream.atEnd()) {
                            quint32 metaSubSize;
                            char metaSubType[5] = {0};

                            if (stream.readRawData((char*)&metaSubSize, 4) != 4) break;
                            metaSubSize = qFromBigEndian(metaSubSize);
                            if (stream.readRawData(metaSubType, 4) != 4) break;

                            if (metaSubSize < 8) break;

                            // TimeZone atom format: 4 bytes atom size, 4 bytes 'tmzn', 2 bytes timezone offset in minutes
                            if (strcmp(metaSubType, "tmzn") == 0 && metaSubSize >= 10) {
                                qint16 tzMinutes;
                                if (stream.readRawData((char*)&tzMinutes, 2) == 2) {
                                    tzMinutes = qFromBigEndian(tzMinutes);
                                    timezoneStr = formatTimezone(tzMinutes);
                                }
                                break;
                            }

                            if (metaSubSize > 8) {
                                file.seek(file.pos() + metaSubSize - 8);
                            }
                        }
                    }

                    // Skip to next atom inside moov
                    if (subAtomSize > 8) {
                        file.seek(file.pos() + subAtomSize - 8);
                    }
                }
                break; // Found moov, no need to search further
            }

            // Skip to next atom
            if (atomSize > 8) {
                file.seek(atomStart + atomSize);
            }
        }

        // If we found both datetime and timezone, no need to search from end of file
        if (creationDateTime.isValid() && !timezoneStr.isEmpty()) {
            break;
        }
    }

    if (!creationDateTime.isValid())
        return QDateTime();

    // QuickTime mvhd stores local time (camera's timezone at recording), not UTC
    // So we don't need to apply the embedded timezone offset - it's already in local time
    // For files without embedded timezone, manual offset will be applied during sorting
    return creationDateTime;
}


DocumentInfo::DocumentInfo(QString path)
    : mDocumentType(DocumentType::NONE),
      mOrientation(0),
      mFormat(""),
      exifLoaded(false),
      mExifDateTime(QDateTime()),
      mExifTimezone("")
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

QDateTime DocumentInfo::exifDateTime() const {
    return mExifDateTime;
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
    loadExifDateTime();
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

void DocumentInfo::loadExifTags() {
    if(exifLoaded)
        return;
    exifLoaded = true;
    exifTags.clear();

    // Ensure datetime is loaded first
    loadExifDateTime();

    // For videos, add creation date if available
    if(mDocumentType == DocumentType::VIDEO && mExifDateTime.isValid()) {
        exifTags.insert(QObject::tr("Date/Time"), mExifDateTime.toString("yyyy:MM:dd HH:mm:ss"));
    }

    // Add timezone information if detected (for all file types)
    if(!mExifTimezone.isEmpty()) {
        exifTags.insert(QObject::tr("Time Zone"), mExifTimezone);
    }

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
        Exiv2::ExifKey userComment("Exif.Photo.UserComment");

        Exiv2::ExifData::const_iterator it;

        it = exifData.findKey(make);
        if(it != exifData.end() /* && it->count() */)
            exifTags.insert(QObject::tr("Make"), QString::fromStdString(it->value().toString()));

        it = exifData.findKey(model);
        if(it != exifData.end())
            exifTags.insert(QObject::tr("Model"), QString::fromStdString(it->value().toString()));

        it = exifData.findKey(dateTime);
        if(it != exifData.end())
            exifTags.insert(QObject::tr("Date/Time"), QString::fromStdString(it->value().toString()));

        it = exifData.findKey(exposureTime);
        if(it != exifData.end()) {
            Exiv2::Rational r = it->toRational();
            if(r.first < r.second) {
                qreal exp = round(static_cast<qreal>(r.second) / r.first);
                exifTags.insert(QObject::tr("ExposureTime"), "1/" + QString::number(exp) + QObject::tr(" sec"));
            } else {
                qreal exp = round(static_cast<qreal>(r.first) / r.second);
                exifTags.insert(QObject::tr("ExposureTime"), QString::number(exp) + QObject::tr(" sec"));
            }
        }

        it = exifData.findKey(fnumber);
        if(it != exifData.end()) {
            Exiv2::Rational r = it->toRational();
            qreal fn = static_cast<qreal>(r.first) / r.second;
            exifTags.insert(QObject::tr("F Number"), "f/" + QString::number(fn, 'g', 3));
        }

        it = exifData.findKey(isoSpeedRatings);
        if(it != exifData.end())
            exifTags.insert(QObject::tr("ISO Speed ratings"), QString::fromStdString(it->value().toString()));

        it = exifData.findKey(flash);
        if(it != exifData.end())
            exifTags.insert(QObject::tr("Flash"), QString::fromStdString(it->value().toString()));

        it = exifData.findKey(focalLength);
        if(it != exifData.end()) {
            Exiv2::Rational r = it->toRational();
            qreal fn = static_cast<qreal>(r.first) / r.second;
            exifTags.insert(QObject::tr("Focal Length"), QString::number(fn, 'g', 3) + QObject::tr(" mm"));
        }

        it = exifData.findKey(userComment);
        if(it != exifData.end()) {
            // crop out 'charset=ascii' etc"
            auto comment = QString::fromStdString(it->value().toString());
            if(comment.startsWith("charset="))
                comment.remove(0, comment.indexOf(" ") + 1);
            exifTags.insert(QObject::tr("UserComment"), comment);
        }
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
#else
    catch (Exiv2::Error& e) {
        qDebug() << "Caught Exiv2 exception:\n" << e.what() << "\n";
        return;
    }
#endif
#endif
}

QMap<QString, QString> DocumentInfo::getExifTags() {
    if(!exifLoaded)
        loadExifTags();
    return exifTags;
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

void DocumentInfo::loadExifDateTime() {
    if(mExifDateTime.isValid())
        return;

    if(mDocumentType == DocumentType::NONE)
        return;

    // Try EXIF/XMP first (which may have timezone info), then fall back to QuickTime atoms
    // This ensures timezone offset is only applied when metadata lacks timezone information

#ifdef USE_EXIV2
    try {
        std::unique_ptr<Exiv2::Image> image;
        image = Exiv2::ImageFactory::open(toStdString(fileInfo.filePath()));

        if(!image.get())
            return;

        image->readMetadata();

        // First try EXIF data (for photos)
        Exiv2::ExifData &exifData = image->exifData();
        if(!exifData.empty()) {
            // Try DateTimeOriginal first (most reliable for photos)
            Exiv2::ExifKey dateTimeOriginal("Exif.Photo.DateTimeOriginal");
            auto it = exifData.findKey(dateTimeOriginal);

            // Try SubSecTimeOriginal for subsecond precision
            Exiv2::ExifKey subSecTimeOriginal("Exif.Photo.SubSecTimeOriginal");
            auto subSecIt = exifData.findKey(subSecTimeOriginal);

            // Try OffsetTimeOriginal for timezone offset
            Exiv2::ExifKey offsetTimeOriginal("Exif.Photo.OffsetTimeOriginal");
            auto offsetIt = exifData.findKey(offsetTimeOriginal);

            // Fallback to DateTime if DateTimeOriginal not found
            if(it == exifData.end()) {
                Exiv2::ExifKey dateTime("Exif.Image.DateTime");
                it = exifData.findKey(dateTime);

                // Also check for subseconds
                if(subSecIt == exifData.end()) {
                    Exiv2::ExifKey subSecTime("Exif.Photo.SubSecTime");
                    subSecIt = exifData.findKey(subSecTime);
                }

                // Also check for offset time
                if(offsetIt == exifData.end()) {
                    Exiv2::ExifKey offsetTime("Exif.Photo.OffsetTime");
                    offsetIt = exifData.findKey(offsetTime);
                }
            }

            if(it != exifData.end()) {
                QString dateTimeStr = QString::fromStdString(it->value().toString());
                // EXIF DateTime format is typically "YYYY:MM:DD HH:MM:SS"
                mExifDateTime = QDateTime::fromString(dateTimeStr, "yyyy:MM:dd HH:mm:ss");

                // Add subsecond precision if available
                if(subSecIt != exifData.end() && mExifDateTime.isValid()) {
                    QString subSec = QString::fromStdString(subSecIt->value().toString());
                    int msec = subSec.left(3).toInt(); // Take first 3 digits for milliseconds
                    mExifDateTime = mExifDateTime.addMSecs(msec);
                }

                // Extract timezone offset if available
                if(offsetIt != exifData.end()) {
                    QString offsetStr = QString::fromStdString(offsetIt->value().toString());
                    if(!offsetStr.isEmpty()) {
                        mExifTimezone = offsetStr;
                    }
                }
            }
        }

        // If EXIF didn't have a date, try XMP data (common for videos)
        if(!mExifDateTime.isValid()) {
            Exiv2::XmpData &xmpData = image->xmpData();
            if(!xmpData.empty()) {
                // First check if there's a separate TimeZone field
                QString timeZoneStr;
                try {
                    Exiv2::XmpKey tzKey("Xmp.video.TimeZone");
                    auto tzIt = xmpData.findKey(tzKey);
                    if(tzIt != xmpData.end()) {
                        timeZoneStr = QString::fromStdString(tzIt->value().toString());
                        if(!timeZoneStr.isEmpty()) {
                            mExifTimezone = timeZoneStr;
                        }
                    }
                } catch(...) {
                    // TimeZone key might not exist
                }

                // Try common XMP date fields
                std::vector<std::string> xmpDateKeys = {
                    "Xmp.xmp.CreateDate",           // XMP creation date
                    "Xmp.video.DateTimeOriginal",   // Video date/time
                    "Xmp.photoshop.DateCreated",    // Photoshop/video date
                    "Xmp.exif.DateTimeOriginal",    // EXIF via XMP
                    "Xmp.xmp.ModifyDate"            // XMP modification date (last resort)
                };

                for(const auto& keyStr : xmpDateKeys) {
                    Exiv2::XmpKey key(keyStr);
                    auto xit = xmpData.findKey(key);
                    if(xit != xmpData.end()) {
                        QString dateTimeStr = QString::fromStdString(xit->value().toString());
                        if(parseXmpDateTime(dateTimeStr, mExifDateTime, mExifTimezone)) {
                            break;
                        }
                    }
                }
            }
        }

        // If still no date, try QuickTime/video metadata (for MP4/MOV videos)
        if(!mExifDateTime.isValid()) {
            // Exiv2 stores QuickTime dates in XMP namespace
            std::vector<std::string> videoDateKeys = {
                "Xmp.video.DateTimeOriginal",
                "Xmp.video.CreateDate",
                "Xmp.video.ModificationDate"
            };

            Exiv2::XmpData &xmpData = image->xmpData();
            for(const auto& keyStr : videoDateKeys) {
                try {
                    Exiv2::XmpKey key(keyStr);
                    auto xit = xmpData.findKey(key);
                    if(xit != xmpData.end()) {
                        QString dateTimeStr = QString::fromStdString(xit->value().toString());
                        if(parseXmpDateTime(dateTimeStr, mExifDateTime, mExifTimezone)) {
                            break;
                        }
                    }
                } catch(...) {
                    // Key might not exist, continue
                    continue;
                }
            }
        }
    }

#if not EXIV2_TEST_VERSION(0, 28, 0)
#ifdef __WIN32
    catch (Exiv2::BasicError<wchar_t>& e) {
        qDebug() << "Caught Exiv2::BasicError exception:\n" << e.what() << "\n";
    }
#else
    catch (Exiv2::BasicError<char>& e) {
        qDebug() << "Caught Exiv2::BasicError exception:\n" << e.what() << "\n";
    }
#endif
#else
    catch (Exiv2::Error& e) {
        qDebug() << "Caught Exiv2 exception:\n" << e.what() << "\n";
    }
#endif
#endif

    // If EXIF/XMP didn't provide a date, OR if it did but lacks timezone info,
    // try QuickTime atom parsing.
    if((!mExifDateTime.isValid() || mExifTimezone.isEmpty()) && mDocumentType == DocumentType::VIDEO) {
        if(mFormat == "mp4" || mFormat == "mov" || mFormat == "m4v") {
            QString qtTimezoneStr;
            QDateTime qtDateTime = readQuickTimeCreationDate(fileInfo.filePath(), qtTimezoneStr);

            // QuickTime stores time in UTC. If we have timezone info, convert to local time.
            if(qtDateTime.isValid() && !qtTimezoneStr.isEmpty()) {
                // Parse timezone offset (e.g., "+02:00" -> 7200 seconds)
                int offsetSeconds = 0;
                if(qtTimezoneStr.length() >= 6) {
                    int hours = qtTimezoneStr.mid(1, 2).toInt();
                    int minutes = qtTimezoneStr.mid(4, 2).toInt();
                    offsetSeconds = (hours * 3600 + minutes * 60);
                    if(qtTimezoneStr[0] == '-') offsetSeconds = -offsetSeconds;
                }
                // Add offset to convert from UTC to local time
                qtDateTime = qtDateTime.addSecs(offsetSeconds);
            }

            // Case 1: We don't have a date yet - use QuickTime's result (with or without timezone)
            if(!mExifDateTime.isValid() && qtDateTime.isValid()) {
                mExifDateTime = qtDateTime;
                if(!qtTimezoneStr.isEmpty()) {
                    mExifTimezone = qtTimezoneStr;
                }
            }
            // Case 2: We have a date from XMP/EXIF but no timezone, AND QuickTime HAS timezone
            // Prefer QuickTime's datetime with timezone over XMP's datetime without timezone
            else if(mExifTimezone.isEmpty() && !qtTimezoneStr.isEmpty() && qtDateTime.isValid()) {
                mExifDateTime = qtDateTime;
                mExifTimezone = qtTimezoneStr;
            }
            // Case 3: We have a date from XMP/EXIF but no timezone, AND QuickTime also has no timezone
            // Keep the XMP/EXIF date as-is.
        }
    }
}
