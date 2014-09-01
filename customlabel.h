#ifndef CustomLabel_H
#define CustomLabel_H

#include <QLabel>
#include <QSize>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QPainter>
#include "image.h"

class CustomLabel : public QLabel
{
    Q_OBJECT
public:
    explicit CustomLabel(QWidget *parent = 0);
    void setImage(QImage*);
    bool kek;
signals:
    void resized();

public slots:
    void resizeEvent(QResizeEvent *event);
    
private:
    QImage* image;

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // CustomLabel_H
