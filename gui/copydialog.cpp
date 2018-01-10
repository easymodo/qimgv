#include "copydialog.h"

CopyDialog::CopyDialog(ContainerWidget *parent) : OverlayWidget(parent) {
    hide();
    this->setFixedSize(290,430);
    this->setContentsMargins(18,18,18,18);

    mLayout.setSpacing(5);
    mLayout.setContentsMargins(0,0,0,0);
    setLayout(&mLayout);

    headerLabel.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    headerLabel.setMargin(0);
    headerLabel.setContentsMargins(0,0,0,6);
    headerLabel.setPixmap(QPixmap(":/res/images/copydialogheader.png"));
    mode = DIALOG_COPY;

    createShortcuts();
    readSettings();
}

void CopyDialog::show() {
    QWidget::show();
    setFocus();
}

void CopyDialog::hide() {
    QWidget::hide();
    clearFocus();
}

CopyDialog::~CopyDialog() {
}

void CopyDialog::setDialogMode(CopyDialogMode _mode) {
    mode = _mode;
    if(mode == DIALOG_COPY)
        headerLabel.setPixmap(QPixmap(":/res/images/copydialogheader.png"));
    else
        headerLabel.setPixmap(QPixmap(":/res/images/movedialogheader.png"));
}

CopyDialogMode CopyDialog::dialogMode() {
    return mode;
}

void CopyDialog::createPathWidgets() {
    mLayout.addWidget(&headerLabel);
    int count = paths.length()>maxPathCount?maxPathCount:paths.length();
    for(int i = 0; i < count; i++) {
        PathSelectorWidget *pathWidget = new PathSelectorWidget(this);
        pathWidget->setPath(paths.at(i));
        pathWidget->setButtonText(shortcuts.key(i));
        connect(pathWidget, SIGNAL(selected(QString)),
                this, SLOT(requestFileOperation(QString)));
        pathWidgets.append(pathWidget);
        mLayout.addWidget(pathWidget, i+1, 0);
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

void CopyDialog::recalculateGeometry() {
    QPoint pos(0, 0);
    pos.setX(25);
    pos.setY(containerSize().height() - height() - 25);
    // apply position
    setGeometry(QRect(pos, size()));
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
