#pragma once

#include <QObject>
#include <memory>
#include "gui/folderview/folderviewproxy.h"
#include "gui/panels/mainpanel/thumbnailstripproxy.h"
#include "components/thumbnailer/thumbnailer.h"
#include "directorymodel.h"
#include "sharedresources.h"

class DirectoryPresenter : public QObject {
    Q_OBJECT
public:
    explicit DirectoryPresenter(QObject *parent = nullptr);

    void setFolderView(std::shared_ptr<FolderViewProxy>);
    void setThumbPanel(std::shared_ptr<ThumbnailStripProxy>);

    void setModel(std::shared_ptr<DirectoryModel> newModel);
    void unsetModel();

    void onIndexChanged(int index);
    void selectAndFocus(int index);

    // not used
    void onFileRemoved(QString filePath, int index);
    void onFileRenamed(QString fromPath, int indexFrom, QString toPath, int indexTo);
    void onFileAdded(QString filePath);
    void onFileModified(QString filePath);

signals:
    void itemActivated(int);

public slots:
    void disconnectAllViews();
    void reloadModel();

private slots:
    void generateThumbnails(QList<int>, int, bool, bool);
    void onThumbnailReady(std::shared_ptr<Thumbnail> thumb, QString filePath);
    void setCurrentIndex(int index);
    void focusOn(int index);
    void populateViews();

private:
    std::shared_ptr<FolderViewProxy> folderView = nullptr;
    std::shared_ptr<ThumbnailStripProxy> thumbPanel = nullptr;
    std::shared_ptr<DirectoryModel> model = nullptr;
    Thumbnailer thumbnailer;
};
