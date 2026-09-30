#pragma once

#include <QString>
#include <QStringList>
#include <QVector>
#include <QRegularExpression>

// The grouping algorithm, in one place: which files belong to the same group (files that
// share a base name, e.g. a raw + its jpeg + its sidecar) and which one of them stands in
// for the whole group. DirectoryManager uses it while building the file list; Core uses it
// to redirect a path opened from the outside onto the file the group is actually shown as.
namespace FileGrouping {

// lower score = higher display priority; entries whose extension isn't
// in the priority list fall back to alphabetical order by extension
QString priorityScore(const QString &path, const QStringList &priorityList);

// the key a file is grouped under: its directory + base name, with every trailing
// extension stripped off sidecar files so a double-extended sidecar (pippo.jpg.xmp)
// lands in the group of the file it belongs to (pippo.jpg) instead of a group of its own
QString groupKey(const QString &path, const QRegularExpression &supportedRegex, const QStringList &priorityList);

// true if path is worth scanning for grouping purposes: either a viewable file, or a
// sidecar whose extension is in the priority list (which can only ever ride along in a group)
bool isGroupable(const QString &path, const QRegularExpression &supportedRegex, const QStringList &priorityList);

// the highest priority viewable file among paths, i.e. the one its group is displayed as.
// Empty if none of them is viewable on its own (e.g. an orphan sidecar).
QString representative(const QVector<QString> &paths, const QRegularExpression &supportedRegex, const QStringList &priorityList);

// scans filePath's directory for the files grouped with it and returns the one representing
// the group. Returns filePath itself if grouping is disabled, or if it already is the
// representative, or if nothing better could be found.
QString resolvePath(const QString &filePath);

}
