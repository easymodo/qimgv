#ifndef OVERLAYCONTAINERWIDGET_H
#define OVERLAYCONTAINERWIDGET_H

#include <QWidget>
#include <memory>
#include <QVBoxLayout>
#include <QDebug>
#include <stack>

class OverlayWidget;

class OverlayContainerWidget : public QWidget {
    Q_OBJECT
public:
    explicit OverlayContainerWidget(QWidget *parent = nullptr);

signals:
    void resized(QSize);

protected:
    void resizeEvent(QResizeEvent *event);
    void focusInEvent(QFocusEvent *event);
    std::stack<OverlayWidget> overlays;
};

#endif // OVERLAYCONTAINERWIDGET_H
