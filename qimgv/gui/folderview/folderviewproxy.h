#pragma once

#include "gui/folderview/folderview.h"

struct FolderViewStateBuffer {
    QString directory;
    QList<int> selection = {-1};
    int itemCount;
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
    virtual void populate(int) Q_DECL_OVERRIDE;
    virtual void setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb) Q_DECL_OVERRIDE;
    virtual void select(QList<int>) Q_DECL_OVERRIDE;
    virtual void select(int) override;
    virtual QList<int> selection() override;
    virtual void focusOn(int) Q_DECL_OVERRIDE;
    virtual void setDirectoryPath(QString path) Q_DECL_OVERRIDE;
    virtual void insertItem(int index) Q_DECL_OVERRIDE;
    virtual void removeItem(int index) Q_DECL_OVERRIDE;
    virtual void reloadItem(int index) Q_DECL_OVERRIDE;
    void addItem();
    void onFullscreenModeChanged(bool mode);
    void onSortingChanged(SortingMode mode);

protected:
    void showEvent(QShowEvent *event) override;

signals:
    void itemActivated(int) override;
    void thumbnailsRequested(QList<int>, int, bool, bool);
    void draggedOut(QList<int>);
    void draggedToBookmarks(QList<int>);
    void sortingSelected(SortingMode);
    void directorySelected(QString);
    void copyUrlsRequested(QList<QUrl>, QString path);
    void moveUrlsRequested(QList<QUrl>, QString path);

private:
    std::shared_ptr<FolderView> folderView;
    QVBoxLayout layout;
    FolderViewStateBuffer stateBuf;
};
