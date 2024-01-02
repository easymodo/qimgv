#include "fsentry.h"

#include <QDir>

FSEntry::FSEntry() {
}

FSEntry::FSEntry(const QString &path) {
    QFileInfo file(path);
    if(file.isDir()) {
        this->name = file.fileName();
        this->path = file.absoluteFilePath();
        this->isDirectory = true;
    } else {
        this->name = file.fileName();
        this->path = file.absoluteFilePath();
        this->isDirectory = false;
        this->size = file.size();
        this->modifyTime = file.lastModified();
    }
}

FSEntry::FSEntry( QString _path, QString _name, std::uintmax_t _size, QDateTime _modifyTime, bool _isDirectory)
    : path(_path),
      name(_name),
      size(_size),
      modifyTime(_modifyTime),
      isDirectory(_isDirectory)
{
}

FSEntry::FSEntry( QString _path, QString _name, std::uintmax_t _size, bool _isDirectory)
    : path(_path),
      name(_name),
      size(_size),
      isDirectory(_isDirectory)
{
}

FSEntry::FSEntry( QString _path, QString _name, bool _isDirectory)
    : path(_path),
      name(_name),
      isDirectory(_isDirectory)
{
}

bool FSEntry::operator==(const QString &anotherPath) const {
    return this->path == anotherPath;
}
