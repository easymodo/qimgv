#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QWidget>
#include <QFileDialog>
#include "mainwindow.h"
#include "imageviewer.h"
#include "directorymanager.h"
#include "opendialog.h"
#include "imageloader.h"

class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core();
    ImageViewer *imageViewer;
    DirectoryManager *dirManager;
    void connectGui(MainWindow*);
    void open(QString);


private:
    void initVariables();
    void connectSlots();
    void initSettings();
    MainWindow *mainWindow;
    OpenDialog *openDialog;
    ImageLoader *imgLoader;

private slots:
    void setInfoString();
signals:

public slots:
    void slotNextImage();
    void slotPrevImage();
    void updateOverlays();
    void setCurrentDir(QString);
    void setDialogDir(QString);
    //from gui
    void showOpenDialog();
};

#endif // CORE_H

