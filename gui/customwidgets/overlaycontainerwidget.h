#ifndef OVERLAYCONTAINERWIDGET_H
#define OVERLAYCONTAINERWIDGET_H

#include <QWidget>
#include <memory>
#include <QVBoxLayout>

class OverlayContainerWidget : public QWidget {
    Q_OBJECT
public:
    explicit OverlayContainerWidget(QWidget *parent = nullptr);

signals:
    void resized(QSize);

protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // OVERLAYCONTAINERWIDGET_H
