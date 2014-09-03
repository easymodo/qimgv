#include "opendialog.h"

OpenDialog::OpenDialog() {
}

void OpenDialog::setDirectory(QString path) {
    dir = path;
}

void OpenDialog::setParent(QWidget* _parent) {
    parent = _parent;
}

QString OpenDialog::getOpenFileName() {
    const QString imagesFilter = tr("Images (*.png *.jpg *jpeg *bmp *gif)");
    return dialog.getOpenFileName(parent,
                                  tr("Open image"),
                                  dir,
                                  imagesFilter,
                                  0);
}
