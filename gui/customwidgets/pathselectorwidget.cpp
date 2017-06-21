#include "pathselectorwidget.h"

PathSelectorWidget::PathSelectorWidget(QWidget *parent) : QWidget(parent) {
    setMinimumSize(200, 40);
    setFixedHeight(40);
    setFocusPolicy(Qt::NoFocus);
    mLayout.setContentsMargins(0,0,0,0);
    mLayout.setSpacing(10);
    setLayout(&mLayout);
    button.setText(" >> ");
    pathLabel.setAccessibleName("PathSelectorLabel");
    pathLabel.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    button.setAccessibleName("PathSelectorButton");
    button.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
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
