#ifndef CUSTOMSCENE_H
#define CUSTOMSCENE_H
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QDebug>

class CustomScene : public QGraphicsScene {
    Q_OBJECT
public:
    CustomScene();
    ~CustomScene();
signals:
    void sceneClick(QPointF);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
};

#endif // CUSTOMSCENE_H
