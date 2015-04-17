#ifndef CUSTOMSCENE_H
#define CUSTOMSCENE_H
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <qDebug>

class CustomScene : public QGraphicsScene {
    Q_OBJECT
public:
    CustomScene();
    ~CustomScene();
signals:
    void click(int);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
};

#endif // CUSTOMSCENE_H
