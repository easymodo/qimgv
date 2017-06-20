#ifndef PATHSELECTORWIDGET_H
#define PATHSELECTORWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDebug>
#include "gui/customwidgets/clickablelabel.h"

class PathSelectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PathSelectorWidget(QWidget *parent = nullptr);
    void setNumber(int _number);
    int number();
    QString path();

signals:
    void selected(QString, int);

public slots:
    void setPath(QString _path);

private slots:
    void pathSelected();
    void showDirectoryChooser();
private:
    ClickableLabel pathLabel;
    QPushButton button;
    QHBoxLayout mLayout;
    int mNumber;
    QString mPath;
};

#endif // PATHSELECTORWIDGET_H
