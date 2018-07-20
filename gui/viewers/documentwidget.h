#ifndef DOCUMENTWIDGET_H
#define DOCUMENTWIDGET_H

#include <memory>
#include <QVBoxLayout>
#include "gui/customwidgets/overlaycontainerwidget.h"
#include "gui/viewers/viewerwidget.h"

// TODO: use a template here?

class DocumentWidget : public OverlayContainerWidget {
public:
    DocumentWidget(std::shared_ptr<ViewerWidget> _viewWidget, QWidget* parent = nullptr);
    std::shared_ptr<ViewerWidget> viewWidget();
private:
    QVBoxLayout layout;
    std::shared_ptr<ViewerWidget> mViewWidget;
};

#endif // DOCUMENTWIDGET_H
