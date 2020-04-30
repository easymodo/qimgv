#include "filesystemmodelcustom.h"

FileSystemModelCustom::FileSystemModelCustom(QObject *parent) : QFileSystemModel(parent) {
    qreal dpr = qApp->devicePixelRatio();
    QString iconPath = ":/res/icons/menuitem/folder16.png";
    if(dpr >= (1.0 + 0.001))
        iconPath.replace(".", "@2x.");
    folderIcon.load(iconPath);
}

QVariant FileSystemModelCustom::data( const QModelIndex& index, int role ) const {
    if(role == Qt::DecorationRole)
        return folderIcon;
    return QFileSystemModel::data(index, role);
}

Qt::ItemFlags FileSystemModelCustom::flags(const QModelIndex& index) const {
    if(!index.isValid()) {
        return 0;
        //return Qt::ItemIsDropEnabled;    // Allow drops in the top-level (no parent)
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
}
