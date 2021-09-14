#pragma once

#include <QApplication>
#include <QLabel>
#include <QBoxLayout>
#include <QScrollArea>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QPainter>
#include <QResizeEvent>
#include <cmath>

#include "gui/customwidgets/thumbnailview.h"
#include "gui/customwidgets/thumbnailwidget.h"
#include "sourcecontainers/thumbnail.h"

class ThumbnailStrip : public ThumbnailView
{
    Q_OBJECT
public:
    explicit ThumbnailStrip(QWidget *parent = nullptr);
    QSize itemSize();

private:
    int thumbPadding = 9, thumbMarginX = 2, thumbMarginY = 2;
    void updateThumbnailPositions(int start, int end);
    void updateThumbnailPositions();
    void setupLayout();
    ThumbnailStyle mCurrentStyle;

public slots:
    virtual void focusOn(int index);
    void readSettings();

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void updateScrollbarIndicator();
    void ensureThumbnailVisible(int pos);
    void addItemToLayout(ThumbnailWidget *widget, int pos);
    void removeItemFromLayout(int pos);
    void removeAll();
    ThumbnailWidget *createThumbnailWidget();
    void ensureSelectedItemVisible();
};
