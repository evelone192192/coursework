#ifndef CANVAS_H
#define CANVAS_H
#include <QGraphicsScene>
#include <vector>
#include <memory>
#include "../Core/Primitive.h"

class Canvas : public QGraphicsScene {
    Q_OBJECT
public:
    explicit Canvas(QObject* parent = nullptr);
    void updateScene(const std::vector<std::unique_ptr<Primitive>>& primitives);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
};
#endif