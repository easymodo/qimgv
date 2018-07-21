#include "centralwidget.h"

CentralWidget::CentralWidget(std::shared_ptr<DocumentWidget> _docWidget, std::shared_ptr<FolderView> _folderView, QWidget *parent)
    : QStackedWidget(parent),
      docWidget(_docWidget),
      folderView(_folderView)
{
    setMouseTracking(true);
    if(!docWidget || !folderView)
        qDebug() << "[CentralWidget] Error: child widget is null. We will crash now.  Bye.";

    // docWidget - 0, folderView - 1
    addWidget(docWidget.get());
    addWidget(folderView.get());
    showDocumentWidget();
}

void CentralWidget::showDocumentWidget() {
    mode = MODE_DOCUMENT;
    setCurrentIndex(0);
    widget(0)->setFocus();
}

void CentralWidget::showFolderView() {
    mode = MODE_FOLDERVIEW;
    setCurrentIndex(1);
    widget(1)->setFocus();
    docWidget->viewWidget()->stopPlayback();
}

CentralWidgetViewMode CentralWidget::viewMode() {
    return mode;
}
