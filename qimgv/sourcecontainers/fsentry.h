#pragma once
#include <QString>
#include <QDateTime>
#include <filesystem>
#include "utils/stuff.h"

class FSEntry {
public:
    FSEntry();
    FSEntry( const QString &filePath);
    FSEntry( QString _path, QString _name, std::uintmax_t _size, QDateTime _modifyTime, bool _isDirectory);
    FSEntry( QString _path, QString _name, std::uintmax_t _size, bool _isDirectory);
    FSEntry( QString _path, QString _name, bool _isDirectory);
    bool operator==(const QString &anotherPath) const;

    QString path, name;
    std::uintmax_t size;
    QDateTime modifyTime;
    bool isDirectory;
};
