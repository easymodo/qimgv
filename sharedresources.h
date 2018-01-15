#ifndef SHAREDRESOURCES_H
#define SHAREDRESOURCES_H

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
    QPixmap *mLoadingIcon72, *mLoadingErrorIcon72;
};

extern SharedResources *shrRes;

#endif // SHAREDRESOURCES_H
