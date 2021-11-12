#include "video.h"
#include <time.h>

Video::Video(QString _path) : Image(_path) {
    Video::load();
}

Video::Video(std::unique_ptr<DocumentInfo> _info) : Image(std::move(_info)) {
    Video::load();
}

void Video::load() {
    if(isLoaded())
        return;

    /*
    auto mpvBinary = settings->mpvBinary();
    if(mpvBinary.isEmpty())
        return;
    // Get resolution from ffmpeg (so we don't have to ask videoplayer)
    QString command = "\"" + mpvBinary + "\"" + " -i " + "\"" + mDocInfo->filePath() + "\"";
    QString filePathEsc = mDocInfo->filePath();
    filePathEsc.replace("%", "%%");
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(settings->mpvBinary(), QStringList() << "-i" << filePathEsc);
    process.waitForFinished(100);
    QByteArray out = process.readAllStandardOutput();

    QRegExp expResolution("[0-9]+x[0-9]+");
    QRegExp expWidth("[0-9]+\\B");
    QRegExp expHeight("\\B+[0-9]+$");
    expResolution.indexIn(out);
    QString res = expResolution.cap();
    expWidth.indexIn(res);
    expHeight.indexIn(res);
    QString wt = expWidth.cap();
    QString ht = expHeight.cap();

    srcWidth = wt.toUInt();
    srcHeight = ht.toUInt();

    qDebug() << "zzzz" << wt << ht << filePathEsc << settings->mpvBinary();
    */
    mLoaded = true;
}

bool Video::save(QString destPath) {
    Q_UNUSED(destPath)
    qDebug() << "Saving video is unsupported.";
    return false;
}

bool Video::save() {
    qDebug() << "Saving video is unsupported.";
    return false;
}

std::unique_ptr<QPixmap> Video::getPixmap() {
    qDebug() << "[Video] getPixmap() is not implemented.";
    //TODO: find out some easy way to get frames from video source
    return nullptr;
}

std::shared_ptr<const QImage> Video::getImage() {
    qDebug() << "[Video] getImage() is not implemented.";
    //TODO: find out some easy way to get frames from video source
    return nullptr;
}

int Video::height() {
    return srcHeight;
}

int Video::width() {
    return srcWidth;
}

QSize Video::size() {
    return QSize(srcWidth, srcHeight);
}
