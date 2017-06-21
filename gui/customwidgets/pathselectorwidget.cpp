#include "pathselectorwidget.h"

PathSelectorWidget::PathSelectorWidget(QWidget *parent) : QWidget(parent) {
    setMinimumSize(200, 36);
    setFixedHeight(36);
    setFocusPolicy(Qt::NoFocus);
    mLayout.setContentsMargins(0,0,0,0);
    mLayout.setSpacing(5);
    setLayout(&mLayout);
    button.setText("?");
    pathLabel.setAccessibleName("PathSelectorLabel");
    pathLabel.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    button.setAccessibleName("PathSelectorButton");
    button.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    button.setFixedSize(36,36);
    button.setAlignment(Qt::AlignCenter);
    font.setPixelSize(15);
    pathLabel.setFont(font);
    font.setBold(true);
    button.setFont(font);

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
    QString stripped = mPath;
    stripped = stripped.split("/", QString::SkipEmptyParts).last() + "/";
    pathLabel.setText(stripped);
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
