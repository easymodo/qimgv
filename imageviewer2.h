#ifndef IMAGEVIEWER2_H
#define IMAGEVIEWER2_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QImageReader>
#include <QTimer>
#include <qshareddata.h>
#include <QDebug>
#include <QErrorMessage>
#include "image.h"
#include <vector>

#include <iostream>
using namespace std;

class AnimationInfo
{
    int mDelay;
    QImage mImage;
    bool nulled;
    
public:
    AnimationInfo() : nulled(true) {}
    AnimationInfo(int delay, const QImage& image);
    bool isNull() const;
    const QImage& image() const;
    int delay();
};

class AnimationList : public vector<AnimationInfo>
{
    int current;
public:
    AnimationList();
    const AnimationInfo& next();
    void reset();
};

class ImageViewer2 : public QWidget
{
    Q_OBJECT

public:
    ImageViewer2();
   // ImageViewer2(QWidget* parent, const QString& image);
    void displayImage(Image*);
    void fitImageHorizontal();
    void fitImageVertical();
    void fitImageDefault();
    void fitImageOriginal();
    void increaseScale(double value);
    void setScale(double scale);
    
private slots:
   // void onAnimation();
    
protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent* event);
    virtual void wheelEvent(QWheelEvent* event);
    
private:
    void animate();
    void recalculateGeometry();
    bool imageInsideWidget(double value);
    
    AnimationList mAnimations;
    QTimer mAnimationTimer;
    QImageReader *mImageReader;
    Image *image;
    QPixmap currentPixmap;
    QPoint mCursorMovedDistance;
    QRect mDrawingRect;
    QSize mShrinkSize;
    
    static const double maxScale = 2.5;
    double mCurrentScale;
};

#endif // IMAGEVIEWER2_H
