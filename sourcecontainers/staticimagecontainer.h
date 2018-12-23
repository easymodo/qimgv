#ifndef STATICIMAGECONTAINER_H
#define STATICIMAGECONTAINER_H

#include <QDebug>
#include <QString>

#include "3rdparty/QtOpenCV/cvmatandqimage.h"

class StaticImageContainer {
public:
    StaticImageContainer(std::shared_ptr<QImage> image);
    StaticImageContainer(std::shared_ptr<cv::Mat> image);
    ~StaticImageContainer();

    const std::shared_ptr<QImage> getImage();

private:
    std::shared_ptr<QImage> imageQt;
    std::shared_ptr<cv::Mat> imageCv;
};

#endif // STATICIMAGECONTAINER_H
