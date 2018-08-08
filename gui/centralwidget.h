#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QStackedWidget>
#include "gui/folderview/folderview.h"
#include "gui/viewers/documentwidget.h"

enum CentralWidgetViewMode {
    MODE_DOCUMENT,
    MODE_FOLDERVIEW
};

class CentralWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit CentralWidget(std::shared_ptr<DocumentWidget> _docWidget, std::shared_ptr<FolderView> _folderView, QWidget *parent = nullptr);

    CentralWidgetViewMode viewMode();
signals:

public slots:
    void showDocumentView();
    void showFolderView();
    void toggleViewMode();

private:
    std::shared_ptr<DocumentWidget> documentView;
    std::shared_ptr<FolderView> folderView;
    CentralWidgetViewMode mode;
};

#endif // CENTRALWIDGET_H
