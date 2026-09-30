#include "filegrouping.h"

#include <QDir>
#include <QFileInfo>

#include "settings.h"

namespace FileGrouping {

QString priorityScore(const QString &path, const QStringList &priorityList) {
    QString ext = QFileInfo(path).suffix().toLower();
    int rank = priorityList.indexOf(ext);
    if(rank >= 0)
        return QStringLiteral("0%1").arg(rank, 6, 10, QChar('0'));
    return QStringLiteral("1%1").arg(ext);
}

QString groupKey(const QString &path, const QRegularExpression &supportedRegex, const QStringList &priorityList) {
    QFileInfo fi(path);
    QString base = fi.completeBaseName();
    // a file that is viewable on its own keeps its plain base name; anything else was only
    // scanned because its extension is in the priority list, i.e. it is a sidecar, so keep
    // peeling extensions off while what remains still names a groupable file
    // (pippo.jpg.xmp -> pippo.jpg -> pippo), leaving names that merely contain dots alone
    // (my.photo.xmp -> my.photo, since "photo" is no extension we know)
    if(!supportedRegex.match(fi.fileName()).hasMatch()) {
        forever {
            QString innerSuffix = QFileInfo(base).suffix().toLower();
            if(innerSuffix.isEmpty() || !(supportedRegex.match(base).hasMatch() || priorityList.contains(innerSuffix)))
                break;
            base = QFileInfo(base).completeBaseName();
        }
    }
    return fi.absolutePath() + "/" + base;
}

bool isGroupable(const QString &path, const QRegularExpression &supportedRegex, const QStringList &priorityList) {
    QString name = QFileInfo(path).fileName();
    return supportedRegex.match(name).hasMatch() ||
           priorityList.contains(QFileInfo(name).suffix().toLower());
}

QString representative(const QVector<QString> &paths, const QRegularExpression &supportedRegex, const QStringList &priorityList) {
    // the representative must be a genuinely supported (viewable) file; entries only
    // grouped because their extension is in the priority list (e.g. a .xmp sidecar)
    // can never be shown on their own, only tucked into a group
    QString repPath, repScore;
    for(const QString &path : paths) {
        if(!supportedRegex.match(QFileInfo(path).fileName()).hasMatch())
            continue;
        QString score = priorityScore(path, priorityList);
        if(repPath.isEmpty() || score < repScore) {
            repPath = path;
            repScore = score;
        }
    }
    return repPath;
}

QString resolvePath(const QString &filePath) {
    if(!settings->groupingEnabled() || filePath.isEmpty())
        return filePath;
    QFileInfo fi(filePath);
    if(!fi.isFile())
        return filePath;

    QRegularExpression supportedRegex(settings->supportedFormatsRegex(),
                                      QRegularExpression::CaseInsensitiveOption);
    QStringList priorityList = settings->groupingExtensionPriorityList();
    QString path = fi.absoluteFilePath();
    QString key = groupKey(path, supportedRegex, priorityList);

    QDir dir(fi.absolutePath());
    QDir::Filters filters = QDir::Files;
    if(settings->showHiddenFiles())
        filters |= QDir::Hidden;
    QVector<QString> group;
    for(const QString &name : dir.entryList(filters)) {
        QString candidate = dir.absoluteFilePath(name);
        if(candidate != path && !isGroupable(candidate, supportedRegex, priorityList))
            continue;
        if(groupKey(candidate, supportedRegex, priorityList) == key)
            group.append(candidate);
    }
    if(!group.contains(path))
        group.append(path);

    QString rep = representative(group, supportedRegex, priorityList);
    return rep.isEmpty() ? filePath : rep;
}

}
