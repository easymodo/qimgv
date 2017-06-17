#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QWidget>
#include <QBoxLayout>
#include <QDebug>
#include "gui/customwidgets/clickablelabel.h"

class ToolBox : public QWidget
{
    Q_OBJECT
public:
    explicit ToolBox(QWidget *parent = 0);
    void readSettings();

private:
    ClickableLabel *rotateLeft, *rotateRight, *zoomFit, *zoomWidth, *zoomOriginal, *zoomIn, *zoomOut;
    QBoxLayout *buttonsLayout;
signals:
    void rotateLeftClicked();
    void rotateRightClicked();
    void zoomFitClicked();
    void zoomWidthClicked();
    void zoomOriginalClicked();
    void zoomInClicked();
    void zoomOutClicked();

public slots:

protected:
    QSize sizeHint();
};

#endif // TOOLBOX_H
