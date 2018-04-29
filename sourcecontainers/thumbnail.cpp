#include "thumbnail.h"

Thumbnail::Thumbnail() : label(""), size(0), image(nullptr) {
}

Thumbnail::~Thumbnail() {
    delete image;
}
