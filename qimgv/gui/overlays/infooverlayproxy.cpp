#include "infooverlayproxy.h"

InfoOverlayProxyWrapper::InfoOverlayProxyWrapper(OverlayContainerWidget *parent)
    : container(parent),
      infoOverlay(nullptr)
{
}

InfoOverlayProxyWrapper::~InfoOverlayProxyWrapper() {
    if(infoOverlay)
        infoOverlay->deleteLater();
}

void InfoOverlayProxyWrapper::show() {
    init();
    infoOverlay->show();
}

void InfoOverlayProxyWrapper::hide() {
    if(infoOverlay)
        infoOverlay->hide();
}

void InfoOverlayProxyWrapper::setInfo(QString _position, QString _fileName, QString _info) {
    if(infoOverlay) {
        infoOverlay->setInfo(_position, _fileName, _info);
    } else {
        stateBuf.position = _position;
        stateBuf.fileName = _fileName;
        stateBuf.info = _info;
    }
}

void InfoOverlayProxyWrapper::init() {
    if(infoOverlay)
        return;
    infoOverlay = new InfoOverlay(container);
    if(!stateBuf.fileName.isEmpty())
        setInfo(stateBuf.position, stateBuf.fileName, stateBuf.info);
}
