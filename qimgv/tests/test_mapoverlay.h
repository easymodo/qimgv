#pragma once

#include <QObject>

class MapOverlay;

class Test_MapOverlay : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();
private:
    bool compare(const QSizeF& a, const QSizeF& b) const;
    
    MapOverlay *minimap;
    
};
