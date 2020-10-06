#pragma once

#include "gui/folderview/folderview.h"

struct FolderViewStateBuffer {
    QString directory;
    QList<int> selection;
    int itemCount = 0;
    SortingMode sortingMode;
    bool fullscreenMode;
};

class FolderViewProxy : public QWidget, public IDirectoryView {
    Q_OBJECT
    Q_INTERFACES(IDirectoryView)
public:
    FolderViewProxy(QWidget *parent = nullptr);
    void init();

public slots:
    virtual void populate(int) override;
    virtual void setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb) override;
    virtual void select(QList<int>) override;
    virtual void select(int) override;
    virtual QList<int> selection() override;
    virtual void focusOn(int) override;
    virtual void setDirectoryPath(QString path) override;
    virtual void insertItem(int index) override;
    virtual void removeItem(int index) override;
    virtual void reloadItem(int index) override;
    void addItem();
    void onFullscreenModeChanged(bool mode);
    void onSortingChanged(SortingMode mode);

protected:
    void showEvent(QShowEvent *event) override;

signals:
    void itemActivated(int) override;
    void thumbnailsRequested(QList<int>, int, bool, bool);
    void draggedOut();
    void draggedToBookmarks(QList<int>);
    void sortingSelected(SortingMode);
    void showFoldersChanged(bool mode);
    void directorySelected(QString);
    void copyUrlsRequested(QList<QString>, QString path);
    void moveUrlsRequested(QList<QString>, QString path);

private:
    std::shared_ptr<FolderView> folderView;
    QVBoxLayout layout;
    FolderViewStateBuffer stateBuf;
};
