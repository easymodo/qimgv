#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "zzscrollarea.h"
#include <QMainWindow>
#include <QDir>


QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void openDialog();
    void zoomIn();
    void zoomOut();
    void fitImage();
    void fitAll();
    void fitWidth();
    void normalSize();
    void next();
    void prev();
    void changeDir(QString);
    
public:
    MainWindow();
    ~MainWindow();
    
private:
    void open(QString);
    void createActions();
    void createMenus();
    void updateActions();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void updateWindowTitle();
    QLabel *imgLabel;
    zzScrollArea *scrollArea;
    QMovie *movie;
    double scaleFactor;
    bool isGif;
   // int gifFrameCount;
    int fileNumber; // # in dir
    QStringList fileList;
    QStringList filters;
    QDir currentDir;
    QFileInfo fileInfo;

    QAction *openAct;
    QAction *nextAct;
    QAction *prevAct;
    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitAllAct;
    QAction *fitWidthAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // MAINWINDOW_H
