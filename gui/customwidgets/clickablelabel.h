#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QWidget>
#include <QLabel>

class ClickableLabel : public QLabel {
    Q_OBJECT
public:
    ClickableLabel();
    ClickableLabel(QWidget *parent);
    ClickableLabel(const QString &text);
    ClickableLabel(const QString &text, QWidget *parent);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // CLICKABLELABEL_H
