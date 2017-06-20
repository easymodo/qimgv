#include "pathselectorwidget.h"

PathSelectorWidget::PathSelectorWidget(QWidget *parent) : QWidget(parent) {
    this->setMinimumSize(200, 35);
    setFixedHeight(35);
    mLayout.setContentsMargins(0,0,0,0);
    mLayout.setSpacing(10);
    setLayout(&mLayout);
    //pathLabel.setFrameStyle(QFrame::Box);
    button.setText(" >> ");
    button.setAutoDefault(true);
    pathLabel.setAccessibleName("PathSelectorLabel");
    button.setAccessibleName("PathSelectorButton");
    this->setFocusPolicy(Qt::NoFocus);
    button.setFocusPolicy(Qt::NoFocus);
    mLayout.addWidget(&pathLabel);
    mLayout.addWidget(&button);
    connect(&pathLabel, SIGNAL(clicked()), this, SLOT(showDirectoryChooser()));
    connect(&button, SIGNAL(clicked()), this, SLOT(pathSelected()));
}

void PathSelectorWidget::setButtonText(QString text) {
    button.setText(text);
}

void PathSelectorWidget::setPath(QString _path) {
    mPath = _path;
    pathLabel.setText(mPath);
}

QString PathSelectorWidget::path() {
    return mPath;
}

void PathSelectorWidget::pathSelected() {
    emit selected(mPath);
}

void PathSelectorWidget::showDirectoryChooser() {
    QFileDialog dialog;
    dialog.setDirectory(mPath);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setWindowTitle("Select directory");
    dialog.setWindowModality(Qt::ApplicationModal);
    connect(&dialog, SIGNAL(fileSelected(QString)),
            this, SLOT(setPath(QString)));
    dialog.exec();
}
