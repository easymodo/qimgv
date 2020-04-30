#pragma once

#include <QObject>
#include <memory>
#include "gui/folderview/folderviewproxy.h"
#include "gui/panels/mainpanel/thumbnailstrip.h"
#include "directorymodel.h"

class DirectoryPresenter : public QObject {
    Q_OBJECT
public:
    explicit DirectoryPresenter(QObject *parent = nullptr);

    void setFolderView(std::shared_ptr<FolderViewProxy>);
    void setThumbPanel(std::shared_ptr<ThumbnailStrip>);

    void setModel(std::shared_ptr<DirectoryModel> newModel);
    void unsetModel();

    void onIndexChanged(int index);
    void selectAndFocus(int index);

    // not used
    void onFileRemoved(QString fileName, int index);
    void onFileRenamed(QString from, int indexFrom, QString to, int indexTo);
    void onFileAdded(QString fileName);
    void onFileModified(QString fileName);

signals:
    void generateThumbnails(QList<int>, int, bool, bool);
    void itemSelected(int);

public slots:
    void disconnectAllViews();
    void reloadModel();

private slots:

    void onThumbnailReady(std::shared_ptr<Thumbnail>);
    void setCurrentIndex(int index);
    void focusOn(int index);
    void populateViews();

private:
    std::shared_ptr<FolderViewProxy> folderView = nullptr;
    std::shared_ptr<ThumbnailStrip> thumbPanel = nullptr;
    std::shared_ptr<DirectoryModel> model = nullptr;
};
