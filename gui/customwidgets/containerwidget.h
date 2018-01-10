/* A QWidget that can be used as a container for overlays.
 */
#ifndef CONTAINERWIDGET_H
#define CONTAINERWIDGET_H

#include <QWidget>

class ContainerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ContainerWidget(QWidget *parent = nullptr);

signals:
    void resized(QSize);

public slots:

protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // CONTAINERWIDGET_H
