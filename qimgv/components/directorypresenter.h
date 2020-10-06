#pragma once

#include <QObject>
#include <memory>
#include "gui/idirectoryview.h"
#include "components/thumbnailer/thumbnailer.h"
#include "directorymodel.h"
#include "sharedresources.h"

//tmp
#include <QtSvg/QSvgRenderer>

class DirectoryPresenter : public QObject {
    Q_OBJECT
public:
    explicit DirectoryPresenter(QObject *parent = nullptr);

    void setView(std::shared_ptr<IDirectoryView>);
    void setModel(std::shared_ptr<DirectoryModel> newModel);
    void unsetModel();

    void selectAndFocus(int index);
    void selectAndFocus(QString path);

    void onFileRemoved(QString filePath, int index);
    void onFileRenamed(QString fromPath, int indexFrom, QString toPath, int indexTo);
    void onFileAdded(QString filePath);
    void onFileModified(QString filePath);

    bool showDirs();
    void setShowDirs(bool mode);

    QList<QString> selectedPaths() const;


signals:
    void dirActivated(QString dirPath);
    void fileActivated(QString filePath);
    void draggedOut(QList<QString>);

public slots:
    void disconnectView();
    void reloadModel();

private slots:
    void generateThumbnails(QList<int>, int, bool, bool);
    void onThumbnailReady(std::shared_ptr<Thumbnail> thumb, QString filePath);
    void populateView();
    void onItemActivated(int absoluteIndex);
    void onDraggedOut();

private:
    std::shared_ptr<IDirectoryView> view = nullptr;
    std::shared_ptr<DirectoryModel> model = nullptr;
    Thumbnailer thumbnailer;
    bool mShowDirs;
};
