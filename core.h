#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QWidget>
#include <QFileDialog>
#include "scrollarea.h"
#include "imageviewer.h"
#include "directorymanager.h"


class Core : public QObject
{
    Q_OBJECT
public:
    explicit Core(QWidget *_parent = 0);
    QWidget* getMainWidget();
    ScrollArea *scrollArea;
    DirectoryManager *dirManager;

private:
    void open(QString);
    QWidget *parent;
signals:

public slots:
    void showNextImage();
    void showPrevImage();
    void updateOverlays();
    void openDialog();
};

#endif // CORE_H

