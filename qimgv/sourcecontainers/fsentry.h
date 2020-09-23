#pragma once
#include <QString>
#include <filesystem>

class FSEntry {
public:
    FSEntry();
    FSEntry( QString _path, QString _name, std::uintmax_t _size, std::filesystem::file_time_type _modifyTime, bool _isDirectory);
    FSEntry( QString _path, QString _name, std::uintmax_t _size, bool _isDirectory);
    FSEntry( QString _path, QString _name, bool _isDirectory);
    bool operator==(const QString &anotherPath) const;

    QString path, name;
    std::uintmax_t size;
    std::filesystem::file_time_type modifyTime;
    bool isDirectory;
};
