#pragma once

#include <QStackedWidget>
#include "gui/folderview/folderviewproxy.h"
#include "gui/viewers/documentwidget.h"

enum ViewMode {
    MODE_DOCUMENT,
    MODE_FOLDERVIEW
};

class CentralWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit CentralWidget(std::shared_ptr<DocumentWidget> _docWidget, std::shared_ptr<FolderViewProxy> _folderView, QWidget *parent = nullptr);

    ViewMode currentViewMode();
signals:

public slots:
    void showDocumentView();
    void showFolderView();
    void toggleViewMode();

private:
    std::shared_ptr<DocumentWidget> documentView;
    std::shared_ptr<FolderViewProxy> folderView;
    ViewMode mode;
};
