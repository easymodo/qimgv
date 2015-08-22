#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QWidget>
#include <QLabel>

class ClickableLabel : public QLabel {
    Q_OBJECT
public:
    ClickableLabel(QWidget *parent);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // CLICKABLELABEL_H
