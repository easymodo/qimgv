#include "pathselectorwidget.h"
#include "ui_pathselectorwidget.h"

PathSelectorWidget::PathSelectorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PathSelectorWidget)
{
    ui->setupUi(this);
    setMinimumSize(200, 10);
    setFocusPolicy(Qt::NoFocus);
    connect(ui->pathLabel, SIGNAL(clicked()), this, SLOT(showDirectoryChooser()));
    connect(ui->button, SIGNAL(clicked()), this, SLOT(pathSelected()));
}

void PathSelectorWidget::setButtonText(QString text) {
    ui->button->setText(text);
}

void PathSelectorWidget::setPath(QString _path) {
    mPath = _path;
    QString stripped = mPath;
    stripped = stripped.split("/", QString::SkipEmptyParts).last() + "/";
    ui->pathLabel->setText(stripped);
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
