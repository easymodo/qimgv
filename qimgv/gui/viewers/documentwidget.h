#ifndef DOCUMENTWIDGET_H
#define DOCUMENTWIDGET_H

#include <memory>
#include <QVBoxLayout>
#include "gui/customwidgets/floatingwidgetcontainer.h"
#include "gui/viewers/viewerwidget.h"
#include "gui/panels/infobar/infobarproxy.h"

// TODO: use a template here?

class DocumentWidget : public FloatingWidgetContainer {
public:
    DocumentWidget(std::shared_ptr<ViewerWidget> viewWidget, std::shared_ptr<InfoBarProxy> infoBar, QWidget* parent = nullptr);
    std::shared_ptr<ViewerWidget> viewWidget();

    void setFocus();
private:
    QVBoxLayout layout;
    std::shared_ptr<ViewerWidget> mViewWidget;
    std::shared_ptr<InfoBarProxy> mInfoBar;
};

#endif // DOCUMENTWIDGET_H
