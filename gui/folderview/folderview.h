#ifndef FOLDERVIEW_H
#define FOLDERVIEW_H

#include <QWidget>
#include "gui/folderview/foldergridview.h"
#include "gui/customwidgets/actionbutton.h"

namespace Ui {
class FolderView;
}

class FolderView : public QWidget
{
    Q_OBJECT

public:
    explicit FolderView(QWidget *parent = nullptr);
    ~FolderView();

public slots:
    void show();
    void hide();
    void populate(int);
    void setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb);
    void selectIndex(int);
    void setDirectoryPath(QString path);
    void addItem();
    void insertItem(int index);
    void removeItem(int index);

    void setCloseButtonEnabled(bool mode);
signals:
    void thumbnailPressed(int);
    void thumbnailRequested(QList<int>, int);

protected:
    void wheelEvent(QWheelEvent *event);
    void focusInEvent(QFocusEvent *event);
    void paintEvent(QPaintEvent *);

protected slots:
    void onShowLabelsChanged(bool mode);
    void onShowLabelsButtonToggled(bool mode);
    void onThumbnailSizeChanged(int newSize);
    void onZoomSliderValueChanged(int value);

private slots:
private:
    Ui::FolderView *ui;
};

#endif // FOLDERVIEW_H
