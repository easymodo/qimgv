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
        infoString.append(currentImage->getFileInfo()->getFileName() + "  ");
        infoString.append("(" +
                          QString::number(currentImage->width()) +
                          "x" +
                          QString::number(currentImage->height()) +
                          "  ");
        infoString.append(QString::number(currentImage->getFileInfo()->getFileSize()) + " MB)");
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

void Core::setCurrentDir(QString path) {
    dirManager->setCurrentDir(path);
}

void Core::slotNextImage() {
    currentImage = NULL;
    FileInfo* f = dirManager->next();
    if(f) {
        imageLoader->load(f);
    }
}

void Core::slotPrevImage() {
    currentImage = NULL;
    FileInfo* f = dirManager->prev();
    if(f) {
        imageLoader->load(f);
    }
}

void Core::loadImage(QString path) {
    if(!path.isEmpty()) {
        currentImage = NULL;
        imageLoader->load(path);
    }
}

void Core::onLoadFinished(Image* img) {
    emit signalUnsetImage();
    qDebug() << "zz";
    currentImage = img;
    emit signalSetImage(currentImage);
    updateInfoString();
}
