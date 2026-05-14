#ifndef CANVAS_H
#define CANVAS_H
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <vector>
#include <memory>
#include "../Core/Primitive.h"

class Canvas : public QGraphicsScene {
    Q_OBJECT
public:
    explicit Canvas(QObject* parent = nullptr);
    void updateScene(const std::vector<std::unique_ptr<Primitive>>& primitives);

signals:
    void lineFinished(Point start, Point end);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    bool m_isDrawing = false;
    QPointF m_startPos;
    QGraphicsLineItem* m_previewLine = nullptr;
};
#endif