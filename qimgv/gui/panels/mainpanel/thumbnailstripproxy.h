#pragma once

#include "gui/panels/mainpanel/thumbnailstrip.h"
#include <QMutexLocker>

struct ThumbnailStripStateBuffer {
    QList<int> selection;
    int itemCount = 0;
};

class ThumbnailStripProxy : public QWidget, public IDirectoryView {
    Q_OBJECT
    Q_INTERFACES(IDirectoryView)
public:
    ThumbnailStripProxy(QWidget *parent = nullptr);
    void init();
    bool isInitialized();
    QSize itemSize();
    void readSettings();

public slots:
    virtual void populate(int) override;
    virtual void setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb) override;
    virtual void select(QList<int>) override;
    virtual void select(int) override;
    virtual QList<int> selection() override;
    virtual void focusOn(int) override;
    virtual void focusOnSelection() override;
    virtual void insertItem(int index) override;
    virtual void removeItem(int index) override;
    virtual void reloadItem(int index) override;
    virtual void setDragHover(int index) override;
    virtual void setDirectoryPath(QString path) override;
    void addItem();

protected:
    void showEvent(QShowEvent *event) override;

signals:
    void itemActivated(int) override;
    void thumbnailsRequested(QList<int>, int, bool, bool) override;
    void draggedOut() override;
    void draggedToBookmarks(QList<int>) override;
    void droppedInto(const QMimeData*, QObject*, int) override;
    void draggedOver(int) override;

private:
    std::shared_ptr<ThumbnailStrip> thumbnailStrip = nullptr;
    QVBoxLayout layout;
    ThumbnailStripStateBuffer stateBuf;
    QMutex m;
};
