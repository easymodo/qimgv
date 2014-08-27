#include "imageviewer.h"

ImageViewer::ImageViewer(QWidget *parent) :
    QScrollArea(parent),
    vBar(this->verticalScrollBar()),
    hBar(this->horizontalScrollBar())
{
    img = NULL;
    label = new CustomLabel(this);
    vBar->setStyleSheet("QScrollBar {height:0px;}");
    hBar->setStyleSheet("QScrollBar {width:0px;}");
    QColor bgColor = QColor(0,0,0,255);
    QPalette bg(bgColor);
    this->setWidget(label);
    this->setMouseTracking(true);
    this->setAlignment(Qt::AlignCenter);
    this->setPalette(bg);
    this->setFrameShape(QFrame::NoFrame);
    this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    fitMode=ALL; //default; later will be defined in settings
}

void ImageViewer::displayImage(Image *_img) {
    if(_img->getType() != NONE) {

        if(img !=NULL && img->getType() == GIF) {
            img->getMovie()->stop();
        }

        delete img;
        img = NULL;
        img = _img;

        if(img->getType()==GIF) {
           label->setMovie(img->getMovie());
            img->getMovie()->start();
        }
        if(img->getType()==STATIC) {
            label->setPixmap(img->getPixmap());
        }
        fitDefault();
     }
}

void ImageViewer::fitAll() {
    fitMode = ALL;
    double viewAspect =
            (double)height()/width();
    QSize newSize;
    if(img->height()>size().height()
            || img->width()>size().width() ) {
        if(img->getAspect()>=viewAspect) {
            newSize.setHeight(height());
            newSize.setWidth(height()/img->getAspect());
        }
        else {
            newSize.setHeight(width()*img->getAspect());
            newSize.setWidth(width());
        }
        label->setFixedSize(newSize);
    }
    else {
        label->setFixedSize(img->size());
    }
}

void ImageViewer::fitWidth() {
    fitMode = WIDTH;
    QSize newSize;
    newSize.setHeight(width()*img->getAspect());
    newSize.setWidth(width());
    label->setFixedSize(newSize);
}

void ImageViewer::fitNormal() {
    fitMode = NORMAL;
    label->setFixedSize(img->size());
}

void ImageViewer::fitDefault() {
    switch(fitMode) {
        case NORMAL: fitNormal(); break;
        case WIDTH: fitWidth(); break;
        case ALL: fitAll(); break;
    }
}

void ImageViewer::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ClosedHandCursor));
        lastDragPos = event->pos();
    }
}

void ImageViewer::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ArrowCursor));
        lastDragPos = event->pos();
    }
}

/* enables mouse dragging
 */
void ImageViewer::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton)
    {
        temp = lastDragPos - event->pos();
        if( (temp.rx()+hBar->value()) > hBar->maximum() ) {
            hBar->setValue(hBar->maximum());
        }
        else {
            hBar->setValue(hBar->value()+temp.rx());
        }

        if( (temp.ry()+vBar->value()) > vBar->maximum() ) {
            vBar->setValue(vBar->maximum());
        }
        else {
            vBar->setValue(vBar->value()+temp.ry());
        }
        lastDragPos = event->pos();
        emit scrollbarChanged();
    }
}

void ImageViewer::mouseDoubleClickEvent(QMouseEvent *event) {
    emit sendDoubleClick();
}

void ImageViewer::wheelEvent(QWheelEvent *event) {
    event->ignore();
}

void ImageViewer::resizeEvent(QResizeEvent *event) {
    if(img!=NULL && fitMode!=NORMAL) {
        fitDefault();
    }
    QScrollArea::resizeEvent(event);
    emit resized();
}

void ImageViewer::keyPressEvent(QKeyEvent *event) {
    event->ignore();
}
