#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "zzscrollarea.h"
#include "infooverlay.h"
#include "fileinfo.h"
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
    void switchFullscreen();
    void receiveDoubleClick();
    void changeDir(QString);
    
public:
    MainWindow();
    ~MainWindow();
    
private:
    void init();
    void open(QString);
    void createActions();
    void createMenus();
    void updateActions();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void updateWindowTitle();
    void updateInfoOverlay();
    QLabel *imgLabel;
    zzScrollArea *scrollArea;
    QMovie *movie;
    QColor bgColor;
    double scaleFactor;
    int fileNumber; // position in directory
    QStringList fileList;
    QStringList filters;
    QDir currentDir;
    fileInfo fInfo;
    QAction *openAct;
    QAction *nextAct;
    QAction *prevAct;
    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitAllAct;
    QAction *fitWidthAct;
    QAction *switchFullscreenAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *navigationMenu;
    QMenu *helpMenu;
    infoOverlay *overlay;

protected:
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);
};

#endif // MAINWINDOW_H
