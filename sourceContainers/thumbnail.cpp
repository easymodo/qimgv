#include "thumbnail.h"

Thumbnail::Thumbnail() : label("") {
}

Thumbnail::~Thumbnail() {
    delete image;
}
