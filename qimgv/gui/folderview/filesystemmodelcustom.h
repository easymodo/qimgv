#ifndef FILESYSTEMMODELCUSTOM_H
#define FILESYSTEMMODELCUSTOM_H

#include <QFileSystemModel>
#include <QApplication>

class FileSystemModelCustom : public QFileSystemModel
{
public:
    FileSystemModelCustom(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const override;

private:
    QPixmap folderIcon;
};

#endif // FILESYSTEMMODELCUSTOM_H
