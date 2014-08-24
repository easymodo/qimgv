#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "scrollarea.h"
#include "imageviewer.h"
#include "infooverlay.h"
#include "controlsoverlay.h"
#include "mapoverlay.h"
#include "fileinfo.h"
#include <QMainWindow>
#include <QDir>
#include <QFile>
#include <QtWidgets>
#include <QAction>
#include <time.h>
#include "image.h"

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
    void open(QString);
    void setCurrentDirectory(QString);
    void zoomIn();
    void zoomOut();
    void fitModeAll();
    void fitModeWidth();
    void fitModeNormal();
    void next();
    void prev();
    void switchFullscreen();
    void triggerFullscreen();
    void minimizeWindow();
    void updateMapOverlay();
    
public:
    MainWindow();
    ~MainWindow();
    
private:
    void setDefaultLogo();
    void init();
    void createActions();
    void createMenus();
    void updateActions();
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void updateWindowTitle();
    void updateInfoOverlay();
    void keyPressEvent(QKeyEvent *event);
    
    ScrollArea *mScrollArea;
    QMovie *movie;
    QColor bgColor;
    double scaleFactor;
    QStringList fileList;
    QStringList filters;
    QDir currentDir;
    QAction *openAct;
    QAction *nextAct;
    QAction *prevAct;
    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *modeFitNormal;
    QAction *modeFitAll;
    QAction *modeFitWidth;
    QAction *fullscreenEnabledAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *navigationMenu;
    QMenu *helpMenu;
    infoOverlay *overlay;
    controlsOverlay *cOverlay;
    mapOverlay *mOverlay;
    bool openFinished;

protected:
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);
    bool eventFilter(QObject *target, QEvent *event);
};

#endif // MAINWINDOW_H
