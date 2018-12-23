#include "staticimagecontainer.h"

StaticImageContainer::StaticImageContainer(std::shared_ptr<cv::Mat> image)
    : imageQt(nullptr),
      imageCv(nullptr)
{
    if(image) {
        imageCv = image;
        imageQt = std::make_shared<QImage>(QtOcv::mat2Image_shared(*imageCv));
    }
}

StaticImageContainer::StaticImageContainer(std::shared_ptr<QImage> image)
    : imageQt(nullptr),
      imageCv(nullptr)
{
    if(image) {
        imageQt = image;
        imageCv = std::make_shared<cv::Mat>(QtOcv::image2Mat_shared(*imageQt));
    }
}

StaticImageContainer::~StaticImageContainer() {
}

const std::shared_ptr<QImage> StaticImageContainer::getImage() {
    return imageQt;
}
