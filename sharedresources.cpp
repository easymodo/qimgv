#include "sharedresources.h"

SharedResources *shrRes = nullptr;

SharedResources::SharedResources()
{
}

SharedResources::~SharedResources() {
    delete shrRes;
}

QPixmap *SharedResources::loadingIcon72() {
    if(!mLoadingIcon72)
        mLoadingIcon72 = new QPixmap(":/res/icons/loading72.png");
    return mLoadingIcon72;
}

QPixmap *SharedResources::loadingErrorIcon72() {
    if(!mLoadingErrorIcon72)
        mLoadingErrorIcon72 = new QPixmap(":/res/icons/loading-error72.png");
    return mLoadingErrorIcon72;
}

SharedResources *SharedResources::getInstance() {
    if(!shrRes) {
        shrRes = new SharedResources();
    }
    return shrRes;
}
