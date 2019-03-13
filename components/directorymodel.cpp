#include "directorymodel.h"

DirectoryModel::DirectoryModel(QObject *parent) : QObject(parent) {
    thumbnailer = new Thumbnailer(&dirManager); // remove pointers
    scaler = new Scaler(&cache);

    connect(&dirManager, SIGNAL(fileRemoved(QString, int)), this, SIGNAL(fileRemoved(QString, int)));
    connect(&dirManager, SIGNAL(fileAdded(QString)), this, SIGNAL(fileAdded(QString)));
    connect(&dirManager, SIGNAL(fileModified(QString)), this, SIGNAL(fileModified(QString)));
    connect(&dirManager, SIGNAL(fileRenamed(QString, QString)), this, SIGNAL(fileRenamed(QString, QString)));
}

DirectoryModel::~DirectoryModel() {
    delete scaler;
    delete thumbnailer;
}
