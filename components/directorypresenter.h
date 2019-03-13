#ifndef DIRECTORYPRESENTER_H
#define DIRECTORYPRESENTER_H

#include <QObject>
#include <memory>
#include "gui/idirectoryview.h"
#include "directorymodel.h"

class DirectoryPresenter : public QObject {
    Q_OBJECT
public:
    explicit DirectoryPresenter(QObject *parent = nullptr);

    void setModel(std::shared_ptr<DirectoryModel> newModel);
    void removeModel();

signals:

public slots:
    void connectView(std::shared_ptr<IDirectoryView> view);

    void disconnectAllViews();
    void disconnectView(std::shared_ptr<IDirectoryView> view);

    void loadByIndex(int);
private slots:
    void onFileRemoved(QString fileName, int index);
    void onFileRenamed(QString from, QString to);
    void onFileAdded(QString fileName);
    void onFileModified(QString fileName);

private:
    QList<std::shared_ptr<IDirectoryView>> views;
    std::shared_ptr<DirectoryModel> model = nullptr;
};

#endif // DIRECTORYPRESENTER_H
