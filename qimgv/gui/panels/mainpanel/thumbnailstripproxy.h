#pragma once

#include "gui/panels/mainpanel/thumbnailstrip.h"

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

public slots:
    virtual void populate(int) override;
    virtual void setThumbnail(int pos, std::shared_ptr<Thumbnail> thumb) override;
    virtual void select(QList<int>) override;
    virtual void select(int) override;
    virtual QList<int> selection() override;
    virtual void focusOn(int) override;
    virtual void insertItem(int index) override;
    virtual void removeItem(int index) override;
    virtual void reloadItem(int index) override;
    virtual void setDirectoryPath(QString path) override;
    void addItem();

protected:
    void showEvent(QShowEvent *event) override;

signals:
    void itemActivated(int) override;
    void thumbnailsRequested(QList<int>, int, bool, bool) override;
    void draggedOut(QList<int>) override;
    void draggedToBookmarks(QList<int>) override;

private:
    std::shared_ptr<ThumbnailStrip> thumbnailStrip;
    QVBoxLayout layout;
    ThumbnailStripStateBuffer stateBuf;
};
