#pragma once

#include <QPixmap>
#include <QDebug>

class SharedResources
{
public:
    SharedResources();
    static SharedResources* getInstance();
    ~SharedResources();

    QPixmap* loadingIcon72();

    QPixmap *loadingErrorIcon72();
private:
    QPixmap *mLoadingIcon72 = nullptr, *mLoadingErrorIcon72 = nullptr;
};

extern SharedResources *shrRes;
