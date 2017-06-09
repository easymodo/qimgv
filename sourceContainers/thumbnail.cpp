#include "thumbnail.h"

Thumbnail::Thumbnail() : label(""), image(NULL), size(0) {
}

Thumbnail::~Thumbnail() {
    delete image;
}
