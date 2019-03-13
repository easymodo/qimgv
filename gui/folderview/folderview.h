#ifndef FOLDERVIEW_H
#define FOLDERVIEW_H

#include <QWidget>
#include "gui/idirectoryview.h"
#include "gui/folderview/foldergridview.h"
#include "gui/customwidgets/actionbutton.h"

namespace Ui {
    class FolderView;
}

class FolderView : public QWidget, public IDirectoryView {
    Q_OBJECT
public:
    explicit FolderView(QWidget *parent = nullptr);
    ~FolderView();

public slots:
    void show();
    void hide();
    virtual void populate(int) Q_DECL_OVERRIDE;
    virtual void setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb) Q_DECL_OVERRIDE;
    virtual void selectIndex(int) Q_DECL_OVERRIDE;
    virtual void setDirectoryPath(QString path) Q_DECL_OVERRIDE;
    virtual void insertItem(int index) Q_DECL_OVERRIDE;
    virtual void removeItem(int index) Q_DECL_OVERRIDE;
    void addItem();
    void setCloseButtonEnabled(bool mode);

protected:
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void focusInEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

protected slots:
    void onShowLabelsChanged(bool mode);
    void onShowLabelsButtonToggled(bool mode);
    void onThumbnailSizeChanged(int newSize);
    void onZoomSliderValueChanged(int value);

signals:
    void thumbnailPressed(int) Q_DECL_OVERRIDE;
    void thumbnailRequested(QList<int>, int) Q_DECL_OVERRIDE;

private slots:
private:
    Ui::FolderView *ui;
};

#endif // FOLDERVIEW_H
