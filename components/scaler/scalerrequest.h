#ifndef SCALERREQUEST_H
#define SCALERREQUEST_H

#include <QPixmap>

class ScalerRequest
{
public:
    ScalerRequest(const QImage *img, QSize sz, QString str) : image(img), size(sz), string(str) {}
    const QImage *image;
    QSize size;
    QString string;
};

#endif // SCALERREQUEST_H
