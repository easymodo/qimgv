#include "thumbnail.h"

Thumbnail::Thumbnail()
    : resLabel(""),
      size(0),
      image(NULL),
      isAnimated(false),
      isVideo(false)
{
}

Thumbnail::~Thumbnail() {
    delete image;
}
