#include "clip.h"

Clip::Clip() {
}

Clip::Clip(const QString &fileName, const char* format) {
    load(fileName, format);
}

Clip::~Clip() {
}

void Clip::load(const QString &fileName, const char* format) {
    QString ffmpegExe = settings->mpvBinary();
    if(ffmpegExe.isEmpty()) {
        return;
    }
    // Get resolution from ffmpeg (so we don't have to ask videoplayer)
    QString command = "\"" + ffmpegExe + "\"" + " -i " + "\"" + fileName + "\"";
    QProcess process;
    process.start(command);
    process.waitForFinished(100);
    QByteArray out = process.readAllStandardError();
    process.close();

    QRegExp expResolution("[0-9]+x[0-9]+");
    QRegExp expWidth("[0-9]+\\B");
    QRegExp expHeight("\\B+[0-9]+$");
    expResolution.indexIn(out);
    QString res = expResolution.cap();
    expWidth.indexIn(res);
    expHeight.indexIn(res);
    QString wt = expWidth.cap();
    QString ht = expHeight.cap();

    path = fileName;
    extension = format;
    srcWidth = wt.toInt();
    srcHeight = ht.toInt();
    grad = 0;
    frame = QRect(0,0,srcWidth, srcHeight);
}

void Clip::save(const QString &fileName) {
    Q_UNUSED(fileName)
    // TODO: Use ffmpeg functions clip and rotate to save video
}

QString Clip::getPath() {
    return path;
}

QTransform Clip::getTransform() {
    return transform;
}

int Clip::height() {
    return frame.height();
}

int Clip::width() {
    return frame.width();
}

QSize Clip::size() {
    return QSize(frame.width(), frame.height());
}

void Clip::rotate(qreal angle) {
    grad += angle;
    transform.rotate(angle);
}

void Clip::setFrame(QRect newFrame) {
    frame = newFrame;
}
