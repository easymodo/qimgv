#include "sharedresources.h"

// TODO: is there a point in doing this? qt does implicit sharing for pixmaps? test

SharedResources *shrRes = nullptr;

SharedResources::SharedResources()
{
}

SharedResources::~SharedResources() {
    delete shrRes;
}

QPixmap *SharedResources::getPixmap(ShrIcon icon, qreal dpr) {
    QPixmap *pixmap;
    QString path;
    if(icon == ShrIcon::SHR_ICON_ERROR) {
        path = ":/res/icons/common/other/loading-error72.png";
        pixmap = mLoadingErrorIcon72;
    } else {
        path = ":/res/icons/common/other/loading72.png";
        pixmap = mLoadingIcon72;
    }
    if(pixmap)
        return pixmap;

    qreal pixmapDrawScale;
    if(dpr >= (1.0 + 0.001)) {
        path.replace(".", "@2x.");
        pixmap = new QPixmap(path);
        if(dpr >= (2.0 - 0.001))
            pixmapDrawScale = dpr;
        else
            pixmapDrawScale = 2.0;
        pixmap->setDevicePixelRatio(pixmapDrawScale);
    } else {
        pixmap = new QPixmap(path);
    }
    if(icon == ShrIcon::SHR_ICON_ERROR)
        mLoadingErrorIcon72 = pixmap;
    else
        mLoadingIcon72 = pixmap;
    return pixmap;
}

SharedResources *SharedResources::getInstance() {
    if(!shrRes) {
        shrRes = new SharedResources();
    }
    return shrRes;
}
