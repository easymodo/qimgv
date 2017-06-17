#include "thumbnail.h"

Thumbnail::Thumbnail() : label(""), size(0), image(NULL) {
}

Thumbnail::~Thumbnail() {
    delete image;
}
