#pragma once

#include <QWidget>
#include <QVBoxLayout>

class FloatingWidgetContainer : public QWidget {
    Q_OBJECT
public:
    explicit FloatingWidgetContainer(QWidget *parent = nullptr);

signals:
    void resized(QSize);

protected:
    void resizeEvent(QResizeEvent *event);
    void focusInEvent(QFocusEvent *event);
};
