#ifndef SCALERREQUEST_H
#define SCALERREQUEST_H

#include <QPixmap>
#include "sourcecontainers/image.h"

class ScalerRequest
{
public:
    ScalerRequest() : image(NULL), size(QSize(0,0)) { }
    ScalerRequest(Image *img, QSize sz, QString str) : image(img), size(sz), string(str) {}
    Image *image;
    QSize size;
    QString string;
};

#endif // SCALERREQUEST_H
