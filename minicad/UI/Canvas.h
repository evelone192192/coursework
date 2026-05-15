#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneWheelEvent>
#include <vector>
#include <memory>
#include <set>
#include <map>
#include "../Core/Primitive.h"

class Canvas : public QGraphicsScene {
    Q_OBJECT
public:
    enum class Tool { Select, Wall, Column, Edit };

    explicit Canvas(QObject* parent = nullptr);

    void updateScene(const std::vector<std::unique_ptr<Primitive>>& primitives,
        const std::set<int>& selectedIds,
        const std::map<int, int>& layerOrder,
        int activeLayerId);

    void setTool(Tool tool);

signals:
    void lineFinished(Point start, Point end);
    void circleFinished(Point center, double radius);
    void objectClicked(Point clickPoint, bool ctrlPressed);
    void objectMoved(double dx, double dy);
    void mouseMoved(Point p);
    void vertexMoved(int id, int vertexIndex, Point newPos);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    Tool m_currentTool = Tool::Wall;
    bool m_isDrawing = false;
    bool m_isDragging = false;
    bool m_isEditingVertex = false;

    int m_activeLayer = 0;
    int m_editingObjectId = -1;
    int m_editingVertexIndex = -1;

    QPointF m_startPos;
    QPointF m_lastMousePos;

    QGraphicsLineItem* m_previewLine = nullptr;
    QGraphicsEllipseItem* m_previewCircle = nullptr;

    QPointF snapPoint(QPointF p);
};

#endif