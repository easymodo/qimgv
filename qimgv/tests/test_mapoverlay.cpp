#include "test_mapoverlay.h"

#include <QtTest>
#include "../overlays/mapoverlay.h"

QTEST_MAIN(Test_MapOverlay);

void Test_MapOverlay::initTestCase() {
    // Called before the first testfunction is executed
    QSize windowSize(200, 100);
    QSizeF drawingSize(1400, 1200);

    QWidget *parent = new QWidget();
    parent->resize(windowSize);

    minimap = new MapOverlay(parent);
    minimap->updateMap(QRectF(QPoint(0, 0), drawingSize));

    /**
     * Drawing area scaled to fit 100x100 rectangle
     * ScaleFactor = 14
     * width  = 100
     * height = 1200 / 14
     */
    compare(minimap->outer(), QSizeF(100, (float) 1200 / 14));

    /**
     * Windows area scaled relative drawing area
     * ScaleFactor = 14
     * width  = 200 / 14
     * height = 100 / 14
     */
    compare(minimap->inner(), QSizeF((float) 200 / 14, (float) 100 / 14));
}

bool Test_MapOverlay::compare(const QSizeF &a, const QSizeF &b) const {
    return (qFuzzyCompare(a.width(), b.width()) && qFuzzyCompare(a.height(), b.height()));
}

void Test_MapOverlay::cleanupTestCase() {
    // Called after the last testfunction was executed
    delete minimap;
}

void Test_MapOverlay::init() {
    // Called before each testfunction is executed
}

void Test_MapOverlay::cleanup() {
    // Called after every testfunction
}

#include "test_mapoverlay.moc"
