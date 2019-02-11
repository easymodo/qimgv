#include "pathselectormenuitem.h"

PathSelectorMenuItem::PathSelectorMenuItem(QWidget *parent)
    : MenuItem(parent),
      mDirectory("")
{
    setFocusPolicy(Qt::NoFocus);
    connect(&mIconLabel, SIGNAL(clicked()), this, SLOT(showDirectoryChooser()));
}

QString PathSelectorMenuItem::directory() {
    return mDirectory;
}

void PathSelectorMenuItem::setDirectory(QString path) {
    mDirectory = path;
    QString stripped = mDirectory.split("/", QString::SkipEmptyParts).last();
    this->mTextLabel.setText(stripped);
}

void PathSelectorMenuItem::onPress() {
    qDebug() << mDirectory;
    if(!mDirectory.isEmpty())
        emit directorySelected(mDirectory);
}

void PathSelectorMenuItem::showDirectoryChooser() {
    QFileDialog dialog;
    dialog.setDirectory(mDirectory);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setWindowTitle("Select directory");
    dialog.setWindowModality(Qt::ApplicationModal);
    connect(&dialog, SIGNAL(fileSelected(QString)),
            this, SLOT(setDirectory(QString)));
    dialog.exec();
}
