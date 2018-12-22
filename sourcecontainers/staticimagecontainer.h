#ifndef STATICIMAGECONTAINER_H
#define STATICIMAGECONTAINER_H

#include <QDebug>
#include <QString>

#include "3rdparty/QtOpenCV/cvmatandqimage.h"

class StaticImageContainer {
public:
    StaticImageContainer(cv::Mat *image);
    StaticImageContainer(QImage *image);
    ~StaticImageContainer();

    const QImage& getImage();

private:
    QImage  *imageQt;
    cv::Mat *imageCv;
};

#endif // STATICIMAGECONTAINER_H
