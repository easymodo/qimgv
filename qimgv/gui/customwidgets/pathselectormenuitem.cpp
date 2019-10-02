#include "pathselectormenuitem.h"

PathSelectorMenuItem::PathSelectorMenuItem(QWidget *parent)
    : MenuItem(parent),
      mDirectory("")
{
    setFocusPolicy(Qt::NoFocus);
    setIcon(QIcon(":/res/icons/buttons/folder16.png"));
    mIconLabel.setAttribute(Qt::WA_TransparentForMouseEvents, false);
    connect(&mIconLabel, &ClickableLabel::clicked, [this]() {
        QFileDialog dialog;
        dialog.setDirectory(mDirectory);
        dialog.setFileMode(QFileDialog::Directory);
        dialog.setWindowTitle("Select directory");
        dialog.setWindowModality(Qt::ApplicationModal);
        connect(&dialog, &QFileDialog::fileSelected, this, &PathSelectorMenuItem::setDirectory);
        dialog.exec();
    });
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
    if(!mDirectory.isEmpty())
        emit directorySelected(mDirectory);
}
