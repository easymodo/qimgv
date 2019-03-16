#ifndef DIRECTORYPRESENTER_H
#define DIRECTORYPRESENTER_H

#include <QObject>
#include <memory>
#include "gui/directoryviewwrapper.h"
#include "directorymodel.h"

class DirectoryPresenter : public QObject {
    Q_OBJECT
public:
    explicit DirectoryPresenter(QObject *parent = nullptr);

    void setModel(std::shared_ptr<DirectoryModel> newModel);
    void removeModel();

signals:
    void generateThumbnails(QList<int>, int);

public slots:
    void connectView(std::shared_ptr<DirectoryViewWrapper>);

    void disconnectAllViews();
    void disconnectView(std::shared_ptr<DirectoryViewWrapper>);

    void loadByIndex(int);
private slots:
    void onFileRemoved(QString fileName, int index);
    void onFileRenamed(QString from, QString to);
    void onFileAdded(QString fileName);
    void onFileModified(QString fileName);

    void reloadModel();
    void onThumbnailReady(std::shared_ptr<Thumbnail>);
private:
    QList<std::shared_ptr<DirectoryViewWrapper>> views;
    std::shared_ptr<DirectoryModel> model = nullptr;
};

#endif // DIRECTORYPRESENTER_H
