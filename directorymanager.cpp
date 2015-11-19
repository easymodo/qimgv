#include "directorymanager.h"

DirectoryManager::DirectoryManager() :
    currentPos(-1),
    startDir(""),
    infiniteScrolling(false),
    quickFormatDetection(true)
{
    readSettings();
    setCurrentDir(startDir);
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(applySettingsChanges()));
}

// ##############################################################
// ####################### PUBLIC METHODS #######################
// ##############################################################

void DirectoryManager::readSettings() {
    infiniteScrolling = settings->infiniteScrolling();
    startDir = settings->lastDirectory();
    if(startDir.isEmpty()) {
        startDir = currentDir.homePath();
    }
    mimeFilters = settings->supportedMimeTypes();
    extensionFilters = settings->supportedFormats();
    generateFileList();
}

void DirectoryManager::setFile(QString path) {
    FileInfo *info = loadInfo(path);
    setCurrentDir(info->getDirectoryPath());
    currentPos = fileNameList.indexOf(info->getFileName());
    settings->setLastFilePosition(currentPos);
}

void DirectoryManager::setCurrentDir(QString path) {
    if(currentDir.exists()) {
        if(currentDir.path() != path) {
            changePath(path);
        }
    } else changePath(path);
}

bool DirectoryManager::setCurrentPos(unsigned int pos) {
    if(containsImages() && pos <= fileCount()) {
        currentPos = pos;
        settings->setLastFilePosition(currentPos);
        return true;
    }
    qDebug() << "DirManager: invalid file position specified (" << pos << ")";
    return false;
}

// full paths array
QStringList DirectoryManager::fileList() {
    QStringList files = fileNameList;
    QString dirPath = currentDir.absolutePath() + "/";
    for(int i = 0; i < fileNameList.length(); i++) {
        files.replace(i, dirPath + files.at(i));
    }
    return files;
}

QString DirectoryManager::currentDirectory() {
    return currentDir.absolutePath();
}

QString DirectoryManager::currentFileName() {
    return fileNameList.at(currentPos);
}

QString DirectoryManager::currentFilePath() {
    QString path = currentDir.absolutePath() + "/" + fileNameList.at(currentFilePos());
    return path;
}

//returns next file's path. does not change current pos
QString DirectoryManager::nextFilePath() {
    QString path = currentDir.absolutePath() + "/" + fileNameList.at(peekNext(1));
    return path;
}

QString DirectoryManager::prevFilePath() {
    QString path = currentDir.absolutePath() + "/" + fileNameList.at(peekPrev(1));
    return path;
}

QString DirectoryManager::filePathAt(int pos) {
    if(pos < 0 || pos > fileNameList.length() - 1) {
        qDebug() << "dirManager: requested index out of range";
        return "";
    }
    QString path = currentDir.absolutePath() + "/" + fileNameList.at(pos);
    return path;
}

int DirectoryManager::currentFilePos() {
    return currentPos;
}

int DirectoryManager::nextPos() {
    currentPos = peekNext(1);
    return currentPos;
}

int DirectoryManager::prevPos() {
    currentPos = peekPrev(1);
    return currentPos;
}

int DirectoryManager::fileCount() {
    return fileNameList.length() - 1;
}

int DirectoryManager::peekNext(int offset) {
    int pos = currentPos + offset;
    if(pos < 0) {
        pos = 0;
    } else if(pos >= fileNameList.length()) {
        if(infiniteScrolling) {
            pos = 0;
        } else {
            pos = fileNameList.length() - 1;
        }
    }
    return pos;
}

int DirectoryManager::peekPrev(int offset) {
    int pos = currentPos - offset;
    if(pos < 0) {
        if(infiniteScrolling) {
            pos = fileNameList.length() - 1;
        } else {
            pos = 0;
        }
    }
    return pos;
}

bool DirectoryManager::existsInCurrentDir(QString file) {
    return fileNameList.contains(file, Qt::CaseInsensitive);
}

bool DirectoryManager::isImage(QString filePath) {
    QFile file(filePath);
    if(file.exists()) {
        QMimeType type = mimeDb.mimeTypeForFile(filePath, QMimeDatabase::MatchContent);
        if(mimeFilters.contains(type.name())) {
            return true;
        }
    }
    return false;
}

bool DirectoryManager::containsImages() {
    return !fileNameList.empty();
}

// ##############################################################
// #######################  PUBLIC SLOTS  #######################
// ##############################################################

void DirectoryManager::applySettingsChanges() {
    infiniteScrolling = settings->infiniteScrolling();
    QDir::SortFlags flags;
    switch(settings->sortingMode()) {
        case 1:
            flags = QDir::SortFlags(QDir::Name | QDir::Reversed | QDir::IgnoreCase);
            break;
        case 2:
            flags = QDir::SortFlags(QDir::Time);
            break;
        case 3:
            flags = QDir::SortFlags(QDir::Time | QDir::Reversed);
            break;
        default:
            flags = QDir::SortFlags(QDir::Name | QDir::IgnoreCase);
            break;
    }
    if(currentDir.sorting() != flags) {
        generateFileList();
        emit directorySortingChanged(); //for now, sorting dir will cause full cache reload TODO
    }
}

// ##############################################################
// ###################### PRIVATE METHODS #######################
// ##############################################################

void DirectoryManager::changePath(QString path) {
    currentDir.setPath(path);
    if(currentDir.isReadable()) {
        settings->setLastDirectory(path);
    } else {
        qDebug() << "DirManager: Invalid directory specified. Removing setting.";
        settings->setLastDirectory("");
    }
    generateFileList();
    currentPos = -1;
    emit directoryChanged(path);
}

FileInfo *DirectoryManager::loadInfo(QString path) {
    FileInfo *info = new FileInfo(path);
    return info;
}

void DirectoryManager::generateFileList() {
    switch(settings->sortingMode()) {
        case 1:
            quickFormatDetection ? generateFileListQuick() : generateFileListDeep();
            naturalSort();
            //currentDir.setSorting(QDir::Name | QDir::Reversed | QDir::IgnoreCase);
            break;
        case 2:
            currentDir.setSorting(QDir::Time);
            quickFormatDetection ? generateFileListQuick() : generateFileListDeep();
            break;
        case 3:
            currentDir.setSorting(QDir::Time | QDir::Reversed);
            quickFormatDetection ? generateFileListQuick() : generateFileListDeep();
            break;
        default:
            quickFormatDetection ? generateFileListQuick() : generateFileListDeep();
            naturalSort();
            //currentDir.setSorting(QDir::Name | QDir::IgnoreCase);
            break;
    }
}

// Filter by file extension, fast.
// Files with unsupported extension are ignored.
// Additionally there is a mime type check on image load (FileInfo::guessType()).
// For example an .exe wont open, but a gif with .jpg extension will still play.
void DirectoryManager::generateFileListQuick() {
    currentDir.setNameFilters(extensionFilters);
    fileNameList = currentDir.entryList();
}

// Filter by mime type. Basically opens every file in a folder
// and checks what's inside. Very slow.
void DirectoryManager::generateFileListDeep() {
    currentDir.setNameFilters(QStringList("*"));
    fileNameList.clear();
    QStringList unfiltered = currentDir.entryList();
    for(int i = 0; i < unfiltered.count(); i++) {
        QString filePath = currentDir.absolutePath() + "/" + unfiltered.at(i);
        if(isImage(filePath)) {
            fileNameList.append(unfiltered.at(i));
        }
    }
}

QVector<QString> massiveFolderRepresentation;

// NOTE: This code is taken from http://kdeblog.mageprojects.com/2014/07/06/natural-string-sorting-an-in-depth-analysis/
// thanks to markg85 for this implementation
// Copied from kstringhandler.cpp in kdecore (part of kdelibs).
int DirectoryManager::naturalCompare(const QString &_a, const QString &_b, Qt::CaseSensitivity caseSensitivity)
{
    // This method chops the input a and b into pieces of
    // digits and non-digits (a1.05 becomes a | 1 | . | 05)
    // and compares these pieces of a and b to each other
    // (first with first, second with second, ...).
    //
    // This is based on the natural sort order code code by Martin Pool
    // http://sourcefrog.net/projects/natsort/
    // Martin Pool agreed to license this under LGPL or GPL.

    // FIXME: Using toLower() to implement case insensitive comparison is
    // sub-optimal, but is needed because we compare strings with
    // localeAwareCompare(), which does not know about case sensitivity.
    // A task has been filled for this in Qt Task Tracker with ID 205990.
    // http://trolltech.com/developer/task-tracker/index_html?method=entry&id=205990
    QString a;
    QString b;
    if (caseSensitivity == Qt::CaseSensitive) {
        a = _a;
        b = _b;
    } else {
        a = _a.toLower();
        b = _b.toLower();
    }

    const QChar* currA = a.unicode(); // iterator over a
    const QChar* currB = b.unicode(); // iterator over b

    if (currA == currB) {
        return 0;
    }

    while (!currA->isNull() && !currB->isNull()) {
        const QChar* begSeqA = currA; // beginning of a new character sequence of a
        const QChar* begSeqB = currB;
        if (currA->unicode() == QChar::ObjectReplacementCharacter) {
            return 1;
        }

        if (currB->unicode() == QChar::ObjectReplacementCharacter) {
            return -1;
        }

        if (currA->unicode() == QChar::ReplacementCharacter) {
            return 1;
        }

        if (currB->unicode() == QChar::ReplacementCharacter) {
            return -1;
        }

        // find sequence of characters ending at the first non-character
        while (!currA->isNull() && !currA->isDigit() && !currA->isPunct() && !currA->isSpace()) {
            ++currA;
        }

        while (!currB->isNull() && !currB->isDigit() && !currB->isPunct() && !currB->isSpace()) {
            ++currB;
        }

        // compare these sequences
        const QStringRef& subA(a.midRef(begSeqA - a.unicode(), currA - begSeqA));
        const QStringRef& subB(b.midRef(begSeqB - b.unicode(), currB - begSeqB));
        const int cmp = QStringRef::localeAwareCompare(subA, subB);
        if (cmp != 0) {
            return cmp < 0 ? -1 : +1;
        }

        if (currA->isNull() || currB->isNull()) {
            break;
        }

        // find sequence of characters ending at the first non-character
        while ((currA->isPunct() || currA->isSpace()) && (currB->isPunct() || currB->isSpace())) {
            if (*currA != *currB) {
                return (*currA < *currB) ? -1 : +1;
            }
            ++currA;
            ++currB;
            if (currA->isNull() || currB->isNull()) {
                break;
            }
        }

        // now some digits follow...
        if ((*currA == QLatin1Char('0')) || (*currB == QLatin1Char('0'))) {
            // one digit-sequence starts with 0 -> assume we are in a fraction part
            // do left aligned comparison (numbers are considered left aligned)
            while (1) {
                if (!currA->isDigit() && !currB->isDigit()) {
                    break;
                } else if (!currA->isDigit()) {
                    return +1;
                } else if (!currB->isDigit()) {
                    return -1;
                } else if (*currA < *currB) {
                    return -1;
                } else if (*currA > *currB) {
                    return + 1;
                }
                ++currA;
                ++currB;
            }
        } else {
            // No digit-sequence starts with 0 -> assume we are looking at some integer
            // do right aligned comparison.
            //
            // The longest run of digits wins. That aside, the greatest
            // value wins, but we can't know that it will until we've scanned
            // both numbers to know that they have the same magnitude.

            bool isFirstRun = true;
            int weight = 0;
            while (1) {
                if (!currA->isDigit() && !currB->isDigit()) {
                    if (weight != 0) {
                        return weight;
                    }
                    break;
                } else if (!currA->isDigit()) {
                    if (isFirstRun) {
                        return *currA < *currB ? -1 : +1;
                    } else {
                        return -1;
                    }
                } else if (!currB->isDigit()) {
                    if (isFirstRun) {
                        return *currA < *currB ? -1 : +1;
                    } else {
                        return +1;
                    }
                } else if ((*currA < *currB) && (weight == 0)) {
                    weight = -1;
                } else if ((*currA > *currB) && (weight == 0)) {
                    weight = + 1;
                }
                ++currA;
                ++currB;
                isFirstRun = false;
            }
        }
    }

    if (currA->isNull() && currB->isNull()) {
        return 0;
    }

    return currA->isNull() ? -1 : + 1;
}

void DirectoryManager::naturalSort()
{
    QVector<QString> localBatch = fileNameList.toVector();

    // Now that we're random.. Sorting time! For this we first setup the QCollator class.
    QCollator col;
    col.setNumericMode(true); // THIS is important. This makes sure that numbers are sorted in a human natural way!
    col.setCaseSensitivity(Qt::CaseInsensitive);

    // Create a timer object. We want to time this!
    QElapsedTimer t;

    t.restart();
    std::vector<QCollatorSortKey> sortKeys; // QVector doesn't work because of the private copy constructor.. It does work with std::vector which probably does move semantics
    QVector<int> keys;
    for(int i = 0; i < fileNameList.count(); i++) {
        sortKeys.emplace_back(col.sortKey(localBatch[i]));
        keys.append(i);
    }
    std::sort(keys.begin(), keys.end(), [&](int a, int b) {
        return sortKeys[a] < sortKeys[b];
    });
    fileNameList.clear();
    for(int i : keys) {
        fileNameList.append(localBatch[i]);
    }
    qDebug() << QString("Done sorting <QCollatorSortKey version>. It took %1 ms").arg(QString::number(t.elapsed()));
}


