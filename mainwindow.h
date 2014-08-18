#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    void fitToWidth();
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
    QLabel *imgLabel;
    QMovie *movie;
    int gifFrameCount;
    int fileNumber; // # in dir
    QStringList fileList;
    QStringList filters;
    double scaleFactor;
    bool isGif;
    QScrollArea *scrollArea;
    QDir currentDir;
    QFileInfo fileInfo;
    QFileInfoList list;

    QAction *openAct;
    QAction *nextAct;
    QAction *prevAct;
    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWidthAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;
};

#endif // MAINWINDOW_H
