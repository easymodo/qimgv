#include "customscene.h"

CustomScene::CustomScene() {

}

CustomScene::~CustomScene()
{

}

void CustomScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    qDebug() << "scene click: " << event->scenePos();
    emit sceneClick(event->scenePos());
}

