#ifndef CustomLabel_H
#define CustomLabel_H

#include <QLabel>
#include <QSize>
#include <QResizeEvent>
#include "image.h"

class CustomLabel : public QLabel
{
    Q_OBJECT
public:
    explicit CustomLabel(QWidget *parent = 0);
    void setPixmap(QPixmap*);
signals:
    void resized();

public slots:
    void resizeEvent(QResizeEvent *event);
    
private:

};

#endif // CustomLabel_H
