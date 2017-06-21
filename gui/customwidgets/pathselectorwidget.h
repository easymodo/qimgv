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
    void setButtonText(QString text);
    QString path();

signals:
    void selected(QString);

public slots:
    void setPath(QString _path);

private slots:
    void pathSelected();
    void showDirectoryChooser();
private:
    ClickableLabel pathLabel;
    ClickableLabel button;
    QHBoxLayout mLayout;
    QString mPath;
    QFont font;
};

#endif // PATHSELECTORWIDGET_H
