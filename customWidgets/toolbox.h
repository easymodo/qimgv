#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QWidget>
#include <QBoxLayout>
#include "clickablelabel.h"
#include <QDebug>

class ToolBox : public QWidget
{
    Q_OBJECT
public:
    explicit ToolBox(QWidget *parent = 0);

private:
    ClickableLabel *rotateLeft, *rotateRight, *zoomFit, *zoomOriginal, *zoomIn, *zoomOut;
    QBoxLayout *buttonsLayout;
signals:
    void rotateLeftClicked();
    void rotateRightClicked();
    void zoomFitClicked();
    void zoomOriginalClicked();
    void zoomInClicked();
    void zoomOutClicked();

public slots:
};

#endif // TOOLBOX_H
