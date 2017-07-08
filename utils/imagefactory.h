#ifndef IMAGEFACTORY_H
#define IMAGEFACTORY_H

#include "utils/imagelib.h"
#include "sourcecontainers/imageinfo.h"
#include "sourcecontainers/image.h"
#include "sourcecontainers/imageanimated.h"
#include "sourcecontainers/imagestatic.h"
#include "sourcecontainers/video.h"

class ImageFactory {
public:
    ImageFactory();

    static Image* createImage(QString) ;
    static Image* createImage(ImageInfo*);
};

#endif // IMAGEFACTORY_H
