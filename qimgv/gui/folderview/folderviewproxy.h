#pragma once

#include "gui/folderview/folderview.h"

struct FolderViewStateBuffer {
    QString directory;
    int selectedIndex = -1;
    int itemCount;
    SortingMode sortingMode;
    bool exitBtn;
};

class FolderViewProxy : public QWidget, public IDirectoryView {
    Q_OBJECT
public:
    FolderViewProxy(QWidget *parent = nullptr);
    void init();
    std::shared_ptr<DirectoryViewWrapper> wrapper();

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
    void setExitButtonEnabled(bool mode);
    void onSortingChanged(SortingMode mode);

protected:
    void showEvent(QShowEvent *event) override;

signals:
    void thumbnailPressed(int) override;
    void thumbnailsRequested(QList<int>, int, bool) override;
    void sortingSelected(SortingMode);

private:
    std::shared_ptr<FolderView> folderView;
    std::shared_ptr<DirectoryViewWrapper> mWrapper;
    QVBoxLayout layout;
    FolderViewStateBuffer stateBuf;
};
