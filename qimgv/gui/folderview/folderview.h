#pragma once

#include <QWidget>
#include <QStyledItemDelegate>
#include <QAbstractItemView>
#include <QFileSystemModel>
#include "gui/idirectoryview.h"
#include "gui/directoryviewwrapper.h"
#include "gui/folderview/foldergridview.h"
#include "gui/folderview/filesystemmodelcustom.h"
#include "gui/customwidgets/actionbutton.h"

namespace Ui {
    class FolderView;
}

class FolderView : public QWidget, public IDirectoryView {
    Q_OBJECT
public:
    explicit FolderView(QWidget *parent = nullptr);
    ~FolderView();

    std::shared_ptr<DirectoryViewWrapper> wrapper();

public slots:
    void show();
    void hide();
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
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void focusInEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

protected slots:
    void onShowLabelsChanged(bool mode);
    void onShowLabelsButtonToggled(bool mode);
    void onThumbnailSizeChanged(int newSize);
    void onZoomSliderValueChanged(int value);

signals:
    void itemSelected(int) Q_DECL_OVERRIDE;
    void thumbnailsRequested(QList<int>, int, bool, bool) Q_DECL_OVERRIDE;
    void sortingSelected(SortingMode);
    void directorySelected(QString path);
    void draggedOut(int index);
    void copyUrlsRequested(QList<QUrl>, QString path);
    void moveUrlsRequested(QList<QUrl>, QString path);

private slots:
    void onSortingSelected(int);
    void readSettings();

    void onTreeViewClicked(QModelIndex index);
    void onDroppedIn(QList<QUrl>, QModelIndex index);
private:
    Ui::FolderView *ui;
    FileSystemModelCustom *dirModel;
    std::shared_ptr<DirectoryViewWrapper> mWrapper;
};
