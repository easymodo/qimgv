#include "customscene.h"

CustomScene::CustomScene() {
}

CustomScene::~CustomScene() {

}

void CustomScene::mousePressEvent (QGraphicsSceneMouseEvent *event) {
    emit sceneClick (event->scenePos());
}
