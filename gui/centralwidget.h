#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QStackedWidget>
#include "gui/folderview/folderview.h"
#include "gui/viewers/documentwidget.h"

class CentralWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit CentralWidget(std::shared_ptr<DocumentWidget> _docWidget, std::shared_ptr<FolderView> _folderView, QWidget *parent = nullptr);

signals:

public slots:
    void showDocumentWidget();
    void showFolderView();

private:
    std::shared_ptr<DocumentWidget> docWidget;
    std::shared_ptr<FolderView> folderView;
};

#endif // CENTRALWIDGET_H
