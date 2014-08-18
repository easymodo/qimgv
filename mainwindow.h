#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <QWidget>

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
    void open();
    void close();
    void zoomIn();
    void zoomOut();
    
public:
    MainWindow();
    ~MainWindow();
    
private:
    QLabel *imgLabel;
    QScrollArea *scrollArea;
};

#endif // MAINWINDOW_H
