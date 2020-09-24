#pragma once

#include <QWidget>
#include <QStyledItemDelegate>
#include <QAbstractItemView>
#include <QFileSystemModel>
#include <QFileDialog>
#include "gui/idirectoryview.h"
#include "gui/folderview/foldergridview.h"
#include "gui/folderview/filesystemmodelcustom.h"
#include "gui/folderview/bookmarkswidget.h"
#include "gui/customwidgets/actionbutton.h"
#include "gui/customwidgets/styledcombobox.h"

namespace Ui {
    class FolderView;
}

class FolderView : public QWidget, public IDirectoryView {
    Q_OBJECT
    Q_INTERFACES(IDirectoryView)
public:
    explicit FolderView(QWidget *parent = nullptr);
    ~FolderView();

public slots:
    void show();
    void hide();
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
    void itemActivated(int) Q_DECL_OVERRIDE;
    void thumbnailsRequested(QList<int>, int, bool, bool) Q_DECL_OVERRIDE;
    void draggedOut(QList<int>) Q_DECL_OVERRIDE;
    void draggedToBookmarks(QList<int>) Q_DECL_OVERRIDE;
    void sortingSelected(SortingMode);
    void directorySelected(QString path);
    void copyUrlsRequested(QList<QUrl>, QString path);
    void moveUrlsRequested(QList<QUrl>, QString path);

private slots:
    void onSortingSelected(int);
    void readSettings();

    void onTreeViewClicked(QModelIndex index);
    void onDroppedInByIndex(QList<QUrl>, QModelIndex index);
    void onDroppedIn(QList<QUrl>, QString dirPath);
    void toggleBookmarks();
    void toggleFilesystemView();
    void setPlacesPanel(bool mode);
    void onPlacesPanelButtonChecked(bool mode);
    void onBookmarkClicked(QString dirPath);
    void newBookmark();
    void fsTreeScrollToCurrent();

    void onSplitterMoved();
    void onHomeBtn();
    void onRootBtn();
private:
    Ui::FolderView *ui;
    FileSystemModelCustom *dirModel;
};
