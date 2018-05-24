#ifndef DOCUMENTWIDGET_H
#define DOCUMENTWIDGET_H

#include <memory>
#include <QVBoxLayout>
#include "gui/customwidgets/overlaycontainerwidget.h"

class DocumentWidget : public OverlayContainerWidget {
public:
    DocumentWidget(QWidget* parent = nullptr);
    DocumentWidget(std::shared_ptr<QWidget> _viewWidget, QWidget* parent = nullptr);
    std::shared_ptr<QWidget> viewWidget();
    void setViewWidget(std::shared_ptr<QWidget> _viewWidget);
private:
    QVBoxLayout layout;
    std::shared_ptr<QWidget> mViewWidget;
};

#endif // DOCUMENTWIDGET_H
