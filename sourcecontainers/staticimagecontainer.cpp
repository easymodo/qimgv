#include "staticimagecontainer.h"

StaticImageContainer::StaticImageContainer(cv::Mat *image)
    : imageQt(nullptr),
      imageCv(nullptr)
{
    if(image) {
        imageCv = image;
        imageQt = new QImage;
        *imageQt = QtOcv::mat2Image_shared(*imageCv);
    }
}

StaticImageContainer::StaticImageContainer(QImage *image)
    : imageQt(nullptr),
      imageCv(nullptr)
{
    if(image) {
        imageQt = image;
        imageCv = new cv::Mat;
        *imageCv = QtOcv::image2Mat_shared(*imageQt);
    }
}

StaticImageContainer::~StaticImageContainer() {
    if(imageQt)
        delete imageQt;
    if(imageCv)
        delete imageCv;
}

const QImage &StaticImageContainer::getImage() {
    return *imageQt;
}
