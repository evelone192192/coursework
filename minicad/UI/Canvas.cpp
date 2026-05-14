#include "Canvas.h"
#include <QGraphicsSceneMouseEvent>
#include <QPen>

Canvas::Canvas(QObject* parent) : QGraphicsScene(parent) {
    setSceneRect(-5000, -5000, 10000, 10000);
}

void Canvas::updateScene(const std::vector<std::unique_ptr<Primitive>>& primitives) {
    this->clear();
    for (const auto& p : primitives) {
        if (p->getType() == Primitive::Type::Line) {
        }
    }
}

void Canvas::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_isDrawing = true;
        m_startPos = event->scenePos();
        m_previewLine = new QGraphicsLineItem(QLineF(m_startPos, m_startPos));
        QPen pen(Qt::blue);
        pen.setStyle(Qt::DashLine);
        m_previewLine->setPen(pen);
        addItem(m_previewLine);
    }
    QGraphicsScene::mousePressEvent(event);
}

void Canvas::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (m_isDrawing && m_previewLine) {
        m_previewLine->setLine(QLineF(m_startPos, event->scenePos()));
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void Canvas::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_isDrawing) {
        m_isDrawing = false;
        if (m_previewLine) {
            removeItem(m_previewLine);
            delete m_previewLine;
            m_previewLine = nullptr;
        }
        emit lineFinished(Point(m_startPos.x(), m_startPos.y()),
            Point(event->scenePos().x(), event->scenePos().y()));
    }
    QGraphicsScene::mouseReleaseEvent(event);
}