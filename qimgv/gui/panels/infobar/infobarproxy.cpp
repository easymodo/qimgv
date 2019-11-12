#include "infobarproxy.h"

InfoBarProxy::InfoBarProxy(QWidget *parent) : QWidget(parent), infoBar(nullptr) {
    setAccessibleName("InfoBarProxy");
    this->setMaximumHeight(30);
    layout.setContentsMargins(0,0,0,0);
    setLayout(&layout);
}

InfoBarProxy::~InfoBarProxy() {
    if(infoBar)
        infoBar->deleteLater();
}

void InfoBarProxy::setInfo(QString position, QString fileName, QString info) {
    if(infoBar) {
        infoBar->setInfo(position, fileName, info);
    } else {
        stateBuf.position = position;
        stateBuf.fileName = fileName;
        stateBuf.info = info;
    }
}

void InfoBarProxy::init() {
    if(infoBar)
        return;
    infoBar = new InfoBar(this);
    setFocusProxy(infoBar);
    layout.addWidget(infoBar);
    setLayout(&layout);
    if(!stateBuf.fileName.isEmpty())
        infoBar->setInfo(stateBuf.position, stateBuf.fileName, stateBuf.info);
}
