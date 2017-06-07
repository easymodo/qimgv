#include "thumbnail.h"

Thumbnail::Thumbnail() : label(""), size(0) {
}

Thumbnail::~Thumbnail() {
    delete image;
}
