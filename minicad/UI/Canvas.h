#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneWheelEvent>
#include <vector>
#include <memory>
#include <set>
#include "../Core/Primitive.h"

class Canvas : public QGraphicsScene {
    Q_OBJECT
public:
    enum class Tool { Select, Line, Circle };

    explicit Canvas(QObject* parent = nullptr);

    void updateScene(const std::vector<std::unique_ptr<Primitive>>& primitives,
        const std::set<int>& selectedIds,
        const std::map<int, int>& layerOrder);

    void setTool(Tool tool);

signals:
    void lineFinished(Point start, Point end);
    void circleFinished(Point center, double radius);
    void objectClicked(Point clickPoint, bool ctrlPressed);
    void objectMoved(double dx, double dy);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

private:
    Tool m_currentTool = Tool::Line;
    bool m_isDrawing = false;
    bool m_isDragging = false;

    QPointF m_startPos;
    QPointF m_lastMousePos;

    QGraphicsLineItem* m_previewLine = nullptr;
    QGraphicsEllipseItem* m_previewCircle = nullptr;
};

#endif