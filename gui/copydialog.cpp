#include "copydialog.h"
#include "ui_copydialog.h"

CopyDialog::CopyDialog(ContainerWidget *parent) :
    FloatingWidget(parent),
    ui(new Ui::CopyDialog)
{
    ui->setupUi(this);
    hide();
    setPosition(FloatingWidgetPosition::BOTTOMLEFT);
    ui->headerLabel->setPixmap(QPixmap(":/res/images/copydialogheader.png"));
    mode = DIALOG_COPY;
    createShortcuts();
    readSettings();
}

CopyDialog::~CopyDialog() {
    delete ui;
}

void CopyDialog::show() {
    QWidget::show();
    setFocus();
}

void CopyDialog::hide() {
    QWidget::hide();
    clearFocus();
}


void CopyDialog::setDialogMode(CopyDialogMode _mode) {
    mode = _mode;
    if(mode == DIALOG_COPY)
        ui->headerLabel->setPixmap(QPixmap(":/res/images/copydialogheader.png"));
    else
        ui->headerLabel->setPixmap(QPixmap(":/res/images/movedialogheader.png"));
}

CopyDialogMode CopyDialog::dialogMode() {
    return mode;
}

void CopyDialog::removePathWidgets() {
    for(int i = 0; i < pathWidgets.count(); i++) {
        QWidget *tmp = pathWidgets.at(i);
        ui->pathSelectorsLayout->removeWidget(tmp);
        delete tmp;
    }
    pathWidgets.clear();
}

void CopyDialog::createPathWidgets() {
    removePathWidgets();
    int count = paths.length()>maxPathCount?maxPathCount:paths.length();
    for(int i = 0; i < count; i++) {
        PathSelectorWidget *pathWidget = new PathSelectorWidget(this);
        pathWidget->setPath(paths.at(i));
        pathWidget->setButtonText(shortcuts.key(i));
        connect(pathWidget, SIGNAL(selected(QString)),
                this, SLOT(requestFileOperation(QString)));
        pathWidgets.append(pathWidget);
        ui->pathSelectorsLayout->addWidget(pathWidget);
    }
}

void CopyDialog::createShortcuts() {
    for(int i=0; i<maxPathCount; i++)
        shortcuts.insert(QString::number(i + 1), i);
}

void CopyDialog::requestFileOperation(QString path) {
    if(mode == DIALOG_COPY)
        emit copyRequested(path);
    else
        emit moveRequested(path);
}

void CopyDialog::requestFileOperation(int fieldNumber) {
    if(mode == DIALOG_COPY)
        emit copyRequested(pathWidgets.at(fieldNumber)->path());
    else
        emit moveRequested(pathWidgets.at(fieldNumber)->path());
}

void CopyDialog::readSettings() {
    paths = settings->savedPaths();
    if(paths.count() < maxPathCount)
        createDefaultPaths();
    createPathWidgets();
    update();
}

void CopyDialog::saveSettings() {
    paths.clear();
    for(int i = 0; i< pathWidgets.count(); i++) {
        paths << pathWidgets.at(i)->path();
    }
    settings->setSavedPaths(paths);
}

void CopyDialog::createDefaultPaths() {
    while(paths.count() < maxPathCount) {
        paths << QDir::homePath();
    }
}

void CopyDialog::keyPressEvent(QKeyEvent *event) {
    QString key = actionManager->keyForNativeScancode(event->nativeScanCode());
    if(shortcuts.contains(key)) {
        event->accept();
        requestFileOperation(shortcuts[key]);
    } else {
        event->ignore();
    }
}
