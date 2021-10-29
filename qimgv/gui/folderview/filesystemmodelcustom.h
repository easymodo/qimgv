#ifndef FILESYSTEMMODELCUSTOM_H
#define FILESYSTEMMODELCUSTOM_H

#include <QApplication>
#include <QFileSystemModel>
#include <QPainter>
#include "settings.h"
#include "utils/imagelib.h"

class FileSystemModelCustom : public QFileSystemModel
{
public:
    FileSystemModelCustom(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const override;

protected:
    Qt::ItemFlags flags(const QModelIndex &index) const override;
private:
    QPixmap folderIcon;
};

#endif // FILESYSTEMMODELCUSTOM_H
