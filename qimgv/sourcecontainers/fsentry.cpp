#include "fsentry.h"

FSEntry::FSEntry() {
}

FSEntry::FSEntry( QString _path, QString _name, std::uintmax_t _size, std::filesystem::file_time_type _modifyTime, bool _isDirectory)
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
