#pragma once

#include "gui/folderview/folderview.h"

struct FolderViewStateBuffer {
    QString directory;
    int selectedIndex = -1;
    int itemCount;
    SortingMode sortingMode;
    bool fullscreenMode;
};

class FolderViewProxy : public QWidget, public IDirectoryView {
    Q_OBJECT
public:
    FolderViewProxy(QWidget *parent = nullptr);
    void init();

public slots:
    virtual void populate(int) Q_DECL_OVERRIDE;
    virtual void setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb) Q_DECL_OVERRIDE;
    virtual void selectIndex(int) Q_DECL_OVERRIDE;
    virtual int selectedIndex() Q_DECL_OVERRIDE;
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
    void sortingSelected(SortingMode);
    void directorySelected(QString);
    void draggedOut(int);
    void copyUrlsRequested(QList<QUrl>, QString path);
    void moveUrlsRequested(QList<QUrl>, QString path);

private:
    std::shared_ptr<FolderView> folderView;
    QVBoxLayout layout;
    FolderViewStateBuffer stateBuf;
};
