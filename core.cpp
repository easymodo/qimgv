#include "core.h"

Core::Core() :
    QObject(),
    imageLoader(NULL),
    dirManager(NULL)
{
    initVariables();
    connectSlots();
    initSettings();
}

const ImageCache *Core::getCache() {
    return imageLoader->getCache();
}

void Core::initVariables() {
    dirManager = new DirectoryManager();
    imageLoader = new ImageLoader(dirManager);
    currentImage = NULL;
}

// misc connections not related to gui
void Core::connectSlots() {
    connect(imageLoader, SIGNAL(loadStarted()),
            this, SLOT(updateInfoString()));
    connect(imageLoader, SIGNAL(loadFinished(Image*)),
            this, SLOT(onLoadFinished(Image*)));
}

void Core::initSettings() {
}

void Core::updateInfoString() {
    QString infoString = "";
    infoString.append(" [ " +
                      QString::number(dirManager->currentPos+1) +
                      "/" +
                      QString::number(dirManager->fileList.length()) +
                      " ]   ");
    if(currentImage) {
        infoString.append(currentImage->getInfo()->getFileName() + "  ");
        infoString.append("(" +
                          QString::number(currentImage->width()) +
                          "x" +
                          QString::number(currentImage->height()) +
                          "  ");
        infoString.append(QString::number(currentImage->getInfo()->getFileSize()) + " MB)");
    }

    emit infoStringChanged(infoString);
}

void Core::rotateImage(int grad) {
    if(currentImage!=NULL) {
        currentImage->rotate(grad);
        updateInfoString();
        emit imageAltered();
    }
}

void Core::crop(QRect newRect) {
    if(currentImage) {
        currentImage->crop(newRect);
        updateInfoString();
    }
    emit imageAltered();
}

void Core::saveImage(QString path) {
    if(currentImage) {
        currentImage->save(path);
    }
}

void Core::saveImage() {
    if(currentImage) {
        currentImage->save();
    }
}

void Core::loadImageByPos(int pos) {
      currentImage = NULL;
      imageLoader->open(pos);
}

void Core::setCurrentDir(QString path) {
    dirManager->setCurrentDir(path);
}

void Core::slotNextImage() {
    currentImage = NULL;
    imageLoader->loadNext();
}

void Core::slotPrevImage() {
    currentImage = NULL;
    imageLoader->loadPrev();
}

void Core::loadImage(QString path) {
    if(!path.isEmpty()) {
        currentImage = NULL;
        imageLoader->open(path);
    }
}

void Core::onLoadFinished(Image* img) {
    emit signalUnsetImage();
    currentImage = img;
    emit signalSetImage(currentImage);
    updateInfoString();
}
