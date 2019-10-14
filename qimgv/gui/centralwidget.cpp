#include "centralwidget.h"

CentralWidget::CentralWidget(std::shared_ptr<DocumentWidget> _docWidget, std::shared_ptr<FolderViewProxy> _folderView, QWidget *parent)
    : QStackedWidget(parent),
      documentView(_docWidget),
      folderView(_folderView)
{
    setMouseTracking(true);
    if(!documentView || !folderView)
        qDebug() << "[CentralWidget] Error: child widget is null. We will crash now.  Bye.";

    // docWidget - 0, folderView - 1
    addWidget(documentView.get());
    if(folderView)
        addWidget(folderView.get());
    showDocumentView();
}

void CentralWidget::showDocumentView() {
    if(mode == MODE_DOCUMENT)
        return;
    mode = MODE_DOCUMENT;
    setCurrentIndex(0);
    widget(0)->setFocus();
    documentView->viewWidget()->startPlayback();
}

void CentralWidget::showFolderView() {
    if(mode == MODE_FOLDERVIEW)
        return;

    mode = MODE_FOLDERVIEW;
    setCurrentIndex(1);
    widget(1)->show();
    widget(1)->setFocus();
    documentView->viewWidget()->stopPlayback();
}

void CentralWidget::toggleViewMode() {
    (mode == MODE_DOCUMENT) ? showFolderView() : showDocumentView();
}

ViewMode CentralWidget::currentViewMode() {
    return mode;
}
