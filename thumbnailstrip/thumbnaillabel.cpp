#include "thumbnaillabel.h"

ThumbnailLabel::ThumbnailLabel() {

}

void ThumbnailLabel::mousePressEvent ( QMouseEvent * event ) {
    Q_UNUSED(event)
    emit clicked(this);
}

ThumbnailLabel::~ThumbnailLabel()
{

}

