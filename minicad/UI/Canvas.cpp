#include "Canvas.h"
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include "../Core/Line.h" 
#include "../Core/Circle.h" 

Canvas::Canvas(QObject* parent) : QGraphicsScene(parent) {
    setSceneRect(-5000, -5000, 10000, 10000);
}

void Canvas::setTool(Tool tool) { m_currentTool = tool; }

void Canvas::updateScene(const std::vector<std::unique_ptr<Primitive>>& primitives, const std::set<int>& selectedIds) {
    this->clear();
    for (const auto& p : primitives) {
        bool isSelected = selectedIds.count(p->getId());
        QPen pen(isSelected ? Qt::red : Qt::black, isSelected ? 3 : 2);

        if (p->getType() == Primitive::Type::Line) {
            Line* l = static_cast<Line*>(p.get());
            this->addLine(l->start().x, l->start().y, l->end().x, l->end().y, pen);
        }
        else if (p->getType() == Primitive::Type::Circle) {
            Circle* c = static_cast<Circle*>(p.get());
            double r = c->radius();
            this->addEllipse(c->center().x - r, c->center().y - r, r * 2, r * 2, pen);
        }
    }
}

void Canvas::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (m_currentTool == Tool::Select) {
            emit objectClicked(Point(event->scenePos().x(), event->scenePos().y()));
        }
        else {
            m_isDrawing = true;
            m_startPos = event->scenePos();
            QPen pen(Qt::blue); pen.setStyle(Qt::DashLine);
            if (m_currentTool == Tool::Line) {
                m_previewLine = new QGraphicsLineItem(QLineF(m_startPos, m_startPos));
                m_previewLine->setPen(pen); addItem(m_previewLine);
            }
            else {
                m_previewCircle = new QGraphicsEllipseItem(QRectF(m_startPos, m_startPos));
                m_previewCircle->setPen(pen); addItem(m_previewCircle);
            }
        }
    }
    QGraphicsScene::mousePressEvent(event);
}

void Canvas::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (m_isDrawing) {
        if (m_currentTool == Tool::Line && m_previewLine)
            m_previewLine->setLine(QLineF(m_startPos, event->scenePos()));
        else if (m_currentTool == Tool::Circle && m_previewCircle) {
            double r = QLineF(m_startPos, event->scenePos()).length();
            m_previewCircle->setRect(m_startPos.x() - r, m_startPos.y() - r, r * 2, r * 2);
        }
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void Canvas::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_isDrawing) {
        m_isDrawing = false;
        if (m_previewLine) { removeItem(m_previewLine); delete m_previewLine; m_previewLine = nullptr; }
        if (m_previewCircle) { removeItem(m_previewCircle); delete m_previewCircle; m_previewCircle = nullptr; }

        if (m_currentTool == Tool::Line)
            emit lineFinished(Point(m_startPos.x(), m_startPos.y()), Point(event->scenePos().x(), event->scenePos().y()));
        else if (m_currentTool == Tool::Circle)
            emit circleFinished(Point(m_startPos.x(), m_startPos.y()), QLineF(m_startPos, event->scenePos()).length());
    }
    QGraphicsScene::mouseReleaseEvent(event);
}