/* A QWidget that displays another QWidget.
 * Can also be used as a parent for OverlayWidget.
 */

#ifndef CONTAINERWIDGET_H
#define CONTAINERWIDGET_H

#include <QWidget>
#include <memory>
#include <QVBoxLayout>

class ContainerWidget : public QWidget {
    Q_OBJECT
public:
    explicit ContainerWidget(QWidget *parent = nullptr);
    explicit ContainerWidget(std::shared_ptr<QWidget> _contentWidget, QWidget *parent = nullptr);
    void setContentWidget(std::shared_ptr<QWidget> w);
    std::shared_ptr<QWidget> contentWidget();

    void removeContentWidget();
signals:
    void resized(QSize);

public slots:

private:
    std::shared_ptr<QWidget> mContentWidget;
    QVBoxLayout layout;

protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // CONTAINERWIDGET_H
