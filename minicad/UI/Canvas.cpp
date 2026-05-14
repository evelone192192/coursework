#include "Canvas.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QPen>
#include <algorithm>
#include "../Core/Line.h" 
#include "../Core/Circle.h" 

Canvas::Canvas(QObject* parent) : QGraphicsScene(parent) {
    setSceneRect(-10000, -10000, 20000, 20000);
}

void Canvas::setTool(Tool tool) {
    m_currentTool = tool;
}

void Canvas::updateScene(const std::vector<std::unique_ptr<Primitive>>& primitives,
    const std::set<int>& selectedIds,
    const std::map<int, int>& layerOrder) {
    this->clear();

    std::vector<Primitive*> sorted;
    for (auto& p : primitives) sorted.push_back(p.get());

    std::sort(sorted.begin(), sorted.end(), [&](Primitive* a, Primitive* b) {
        int priorityA = layerOrder.count(a->getLayerId()) ? layerOrder.at(a->getLayerId()) : 0;
        int priorityB = layerOrder.count(b->getLayerId()) ? layerOrder.at(b->getLayerId()) : 0;
        return priorityA < priorityB;
        });

    for (auto* p : sorted) {
        bool isSelected = selectedIds.count(p->getId());
        QColor baseColor = QColor::fromRgba(p->getColor());
        QPen pen(isSelected ? Qt::red : baseColor, isSelected ? 3 : 2);

        if (p->getType() == Primitive::Type::Line) {
            Line* l = static_cast<Line*>(p);
            this->addLine(l->start().x, l->start().y, l->end().x, l->end().y, pen);
        }
        else if (p->getType() == Primitive::Type::Circle) {
            Circle* c = static_cast<Circle*>(p);
            double r = c->radius();
            this->addEllipse(c->center().x - r, c->center().y - r, r * 2, r * 2, pen);
        }
    }
}

void Canvas::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_startPos = event->scenePos();
        m_lastMousePos = m_startPos;

        if (m_currentTool == Tool::Select) {
            bool ctrl = event->modifiers() & Qt::ControlModifier;
            emit objectClicked(Point(m_startPos.x(), m_startPos.y()), ctrl);
            m_isDragging = true;
        }
        else {
            m_isDrawing = true;
            QPen pen(Qt::blue); pen.setStyle(Qt::DashLine);
            if (m_currentTool == Tool::Line) {
                m_previewLine = new QGraphicsLineItem(QLineF(m_startPos, m_startPos));
                m_previewLine->setPen(pen); addItem(m_previewLine);
            }
            else if (m_currentTool == Tool::Circle) {
                m_previewCircle = new QGraphicsEllipseItem(QRectF(m_startPos, m_startPos));
                m_previewCircle->setPen(pen); addItem(m_previewCircle);
            }
        }
    }
    QGraphicsScene::mousePressEvent(event);
}

void Canvas::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    QPointF currentPos = event->scenePos();

    if (m_isDragging && m_currentTool == Tool::Select) {
        double dx = currentPos.x() - m_lastMousePos.x();
        double dy = currentPos.y() - m_lastMousePos.y();
        emit objectMoved(dx, dy);
        m_lastMousePos = currentPos;
    }
    else if (m_isDrawing) {
        if (m_currentTool == Tool::Line && m_previewLine) {
            m_previewLine->setLine(QLineF(m_startPos, currentPos));
        }
        else if (m_currentTool == Tool::Circle && m_previewCircle) {
            double r = QLineF(m_startPos, currentPos).length();
            m_previewCircle->setRect(m_startPos.x() - r, m_startPos.y() - r, r * 2, r * 2);
        }
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void Canvas::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (m_isDragging) {
            m_isDragging = false;
        }
        if (m_isDrawing) {
            m_isDrawing = false;
            if (m_previewLine) { removeItem(m_previewLine); delete m_previewLine; m_previewLine = nullptr; }
            if (m_previewCircle) { removeItem(m_previewCircle); delete m_previewCircle; m_previewCircle = nullptr; }

            if (m_currentTool == Tool::Line) {
                emit lineFinished(Point(m_startPos.x(), m_startPos.y()),
                    Point(event->scenePos().x(), event->scenePos().y()));
            }
            else if (m_currentTool == Tool::Circle) {
                double r = QLineF(m_startPos, event->scenePos()).length();
                emit circleFinished(Point(m_startPos.x(), m_startPos.y()), r);
            }
        }
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void Canvas::wheelEvent(QGraphicsSceneWheelEvent* event) {
    if (event->modifiers() & Qt::ControlModifier) {
        if (views().isEmpty()) return;
        QGraphicsView* view = views().first();

        double scaleFactor = 1.15;
        if (event->delta() > 0) {
            view->scale(scaleFactor, scaleFactor);
        }
        else {
            view->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
        event->accept();
    }
    else {
        QGraphicsScene::wheelEvent(event);
    }
}