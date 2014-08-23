#include "directorymanager.h"

DirectoryManager::DirectoryManager() :
    aspectRatio(1),
    currentPosition(-1)
{
    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.gif" << "*.bmp";
    currentDir.setNameFilters(filters);
    clear();
}

void DirectoryManager::changeCurrentDir(QString path) {
    currentDir.setCurrent(path);
    currentDir.setNameFilters(filters);
    fileList = currentDir.entryList();
    fInfo.currentPosition = -1;
}

void DirectoryManager::next() {
    if(currentDir.exists() && fileList.length()) {
        if(++fileInfo.currentPosition>=fileList.length()) {
            fileInfo.currentPosition=0;
        }
        QString fileName = currentDir.currentPath()
                        +"/"
                        +fileList.at(fileInfo.currentPosition);
        fileInfo.setFile(fileName);
        open(fileName); //MOVE
    }
}

void DirectoryManager::prev() {
    if(currentDir.exists() && fileList.length()) {
        if(--fileInfo.currentPosition<0) {
            fileInfo.currentPosition=fileList.length()-1;
        }
        QString fileName = currentDir.currentPath()
                        +"/"
                        +fileList.at(fileInfo.currentPosition);
        fileInfo.setFile(fileName);
        open(fileName); //MOVE
    }
}

bool DirectoryManager::setFile(QString _path) {
    fileInfo.setFile(_path);
    if(fileInfo.isFile()) {
        QFile file(fileInfo.filePath());
        file.open(QIODevice::ReadOnly);
        //read first 2 bytes to determine file format
        QByteArray startingBytes= file.read(2).toHex();
        qDebug() << startingBytes;
        if(startingBytes == "4749") {
            fInfo.type = GIF;
        }
        else if(startingBytes == "ffd8"
                || startingBytes == "8950"
                || startingBytes == "424d") {
            fInfo.type = STATIC;
        }
        else {
            type = NONE;
        }
        return fileInfo.isFile();
    }
}

void DirectoryManager::setFileDimensions(QSize dimensions) {
    fileDimensions=dimensions;
    if(fileDimensions->width() && fileDimensions->height()) {
        aspectRatio = (double)fileDimensions.height()
                      / fileDimensions.width();
    }
    else {
        aspectRatio = 1;
    }
}

void DirectoryManager::clearFileInfo() {
    setFile("");
    type = NONE;
    setFileDimensions(QSize(0,0));
}

QString DirectoryManager::getInfo() {
    if(!fileInfo.exists() || type == NONE)
        return "No file opened  ";
    QString inf = fileInfo.fileName()
                  + "  ("+QString::number(size.width())
                  + " x "
                  + QString::number(size.height())
                  + ",  "
                  + QString::number(qInfo.size()/1024)
                  + " KB)  ";
    return inf;
}
