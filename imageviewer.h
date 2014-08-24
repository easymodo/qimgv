#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QLabel>
#include <QSize>
#include <QResizeEvent>
#include "image.h"

class ImageViewer : public QLabel
{
    Q_OBJECT
public:
    explicit ImageViewer(QWidget *parent = 0);
    QString getImagePath() const;
    double getAspect() const;
    bool setImagePath(const QString& path);
    QString getImageInfo() const;
    void fitOriginalSize();
signals:
    void resized();

public slots:
    void resizeEvent(QResizeEvent *event);
    
private:
    void setPixmap(const QPixmap&);
    void setMovie(QMovie*);
    
    Image* mImage;
    QSize mSize;
    double mAspect;
};

#endif // IMAGEVIEWER_H
