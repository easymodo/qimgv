#include "sharedresources.h"

SharedResources *shrRes = NULL;

SharedResources::SharedResources()
{
    mLoadingIcon72 = new QPixmap(":/res/icons/loading72.png");
    mLoadingErrorIcon72 = new QPixmap(":/res/icons/loading-error72.png");
}

SharedResources::~SharedResources() {
    delete shrRes;
}

QPixmap *SharedResources::loadingIcon72() {
    return mLoadingIcon72;
}

QPixmap *SharedResources::loadingErrorIcon72() {
    return mLoadingErrorIcon72;
}

SharedResources *SharedResources::getInstance() {
    if(!shrRes) {
        shrRes = new SharedResources();
    }
    return shrRes;
}
