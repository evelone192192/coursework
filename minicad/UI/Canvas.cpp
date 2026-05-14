#include "Canvas.h"
#include <QGraphicsSceneMouseEvent>

Canvas::Canvas(QObject* parent) : QGraphicsScene(parent) {}

void Canvas::updateScene(const std::vector<std::unique_ptr<Primitive>>& primitives) {
    this->clear();
}

void Canvas::mousePressEvent(QGraphicsSceneMouseEvent* event) { QGraphicsScene::mousePressEvent(event); }
void Canvas::mouseMoveEvent(QGraphicsSceneMouseEvent* event) { QGraphicsScene::mouseMoveEvent(event); }
void Canvas::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) { QGraphicsScene::mouseReleaseEvent(event); }