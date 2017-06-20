#include "copydialog.h"

CopyDialog::CopyDialog(QWidget *parent) : OverlayWidget(parent) {
    hide();
    this->setFixedSize(500,500);
    this->setContentsMargins(30,20,30,20);

    mLayout.setSpacing(12);
    mLayout.setContentsMargins(0,0,0,0);
    setLayout(&mLayout);

    // drawing stuff
    borderPen.setWidth(7);

    maxPathCount = 9;

    readSettings();
}

CopyDialog::~CopyDialog() {
}

void CopyDialog::createPathWidgets() {
    int count = paths.length()>maxPathCount?maxPathCount:paths.length();
    for(int i = 0; i < count; i++) {
        PathSelectorWidget *pathWidget = new PathSelectorWidget(this);
        pathWidget->setPath(paths.at(i));
        pathWidget->setNumber(i+1);
        connect(pathWidget, SIGNAL(selected(QString,int)),
                this, SLOT(requestCopy(QString,int)));
        pathWidgets.append(pathWidget);
        mLayout.addWidget(pathWidget, i, 0);
    }
}

void CopyDialog::requestCopy(QString path, int number) {
    emit copyRequested(path);
}

void CopyDialog::readSettings() {
    borderPen.setColor(settings->accentColor());
    bgColor = settings->backgroundColor();
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
    //pos.setX( (containerSize().width() - width()) / 2);
    //pos.setY(containerSize().height() - height() - 25);
    // apply position
    setGeometry(QRect(pos, size()));
}

void CopyDialog::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter p(this);
    p.fillRect(rect(), bgColor);
    p.setPen(borderPen);
    p.drawRect(rect().adjusted(0,0,-1,-1));
}
