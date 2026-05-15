#include "Canvas.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QPainter>
#include <QPen>
#include <algorithm>
#include <cmath>
#include "../Core/Line.h" 
#include "../Core/Circle.h" 

Canvas::Canvas(QObject* parent) : QGraphicsScene(parent) {
    setBackgroundBrush(Qt::white);
    setSceneRect(-50000, -50000, 100000, 100000);
}

QPointF Canvas::snapPoint(QPointF p) {
    const int step = 25;
    return QPointF(qRound(p.x() / step) * step, qRound(p.y() / step) * step);
}

void Canvas::setTool(Tool tool) { m_currentTool = tool; }

void Canvas::drawBackground(QPainter* painter, const QRectF& rect) {
    painter->fillRect(rect, Qt::white);

    const int gridSize = 50;
    qreal left = std::floor(rect.left() / gridSize) * gridSize;
    qreal top = std::floor(rect.top() / gridSize) * gridSize;

    QPen gridPen(QColor(230, 230, 230), 0.5);
    painter->setPen(gridPen);

    for (qreal x = left; x < rect.right(); x += gridSize)
        painter->drawLine(x, rect.top(), x, rect.bottom());
    for (qreal y = top; y < rect.bottom(); y += gridSize)
        painter->drawLine(rect.left(), y, rect.right(), y);

    painter->setPen(QPen(QColor(255, 0, 0, 100), 1));
    painter->drawLine(0, rect.top(), 0, rect.bottom());
    painter->setPen(QPen(QColor(0, 0, 255, 100), 1));
    painter->drawLine(rect.left(), 0, rect.right(), 0);
}

void Canvas::updateScene(const std::vector<std::unique_ptr<Primitive>>& primitives,
    const std::set<int>& selectedIds,
    const std::map<int, int>& layerOrder) {
    this->clear();
    std::vector<Primitive*> sorted;
    for (auto& p : primitives) sorted.push_back(p.get());

    std::sort(sorted.begin(), sorted.end(), [&](Primitive* a, Primitive* b) {
        int prA = layerOrder.count(a->getLayerId()) ? layerOrder.at(a->getLayerId()) : 0;
        int prB = layerOrder.count(b->getLayerId()) ? layerOrder.at(b->getLayerId()) : 0;
        return prA < prB;
        });

    for (auto* p : sorted) {
        bool isSel = selectedIds.count(p->getId());
        QPen pen(isSel ? Qt::red : QColor::fromRgba(p->getColor()), isSel ? 3 : 2);
        if (p->getType() == Primitive::Type::Line) {
            Line* l = static_cast<Line*>(p);
            this->addLine(l->start().x, l->start().y, l->end().x, l->end().y, pen);
        }
        else {
            Circle* c = static_cast<Circle*>(p);
            double r = c->radius();
            this->addEllipse(c->center().x - r, c->center().y - r, r * 2, r * 2, pen);
        }
    }
}

void Canvas::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_startPos = snapPoint(event->scenePos());
        m_lastMousePos = m_startPos;

        if (m_currentTool == Tool::Select) {
            emit objectClicked(Point(m_startPos.x(), m_startPos.y()), event->modifiers() & Qt::ControlModifier);
            m_isDragging = true;
        }
        else {
            m_isDrawing = true;
            QPen p(Qt::blue, 1, Qt::DashLine);
            if (m_currentTool == Tool::Wall) {
                m_previewLine = new QGraphicsLineItem(QLineF(m_startPos, m_startPos));
                m_previewLine->setPen(p); addItem(m_previewLine);
            }
            else {
                m_previewCircle = new QGraphicsEllipseItem(QRectF(m_startPos, m_startPos));
                m_previewCircle->setPen(p); addItem(m_previewCircle);
            }
        }
    }
    QGraphicsScene::mousePressEvent(event);
}

void Canvas::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    QPointF currentPos = snapPoint(event->scenePos());
    emit mouseMoved(Point(currentPos.x(), currentPos.y()));

    if (m_isDragging && m_currentTool == Tool::Select) {
        double dx = currentPos.x() - m_lastMousePos.x();
        double dy = currentPos.y() - m_lastMousePos.y();
        if (std::abs(dx) > 0 || std::abs(dy) > 0) {
            emit objectMoved(dx, dy);
            m_lastMousePos = currentPos;
        }
    }
    else if (m_isDrawing) {
        if (m_currentTool == Tool::Wall && m_previewLine)
            m_previewLine->setLine(QLineF(m_startPos, currentPos));
        else if (m_currentTool == Tool::Column && m_previewCircle) {
            double r = QLineF(m_startPos, currentPos).length();
            m_previewCircle->setRect(m_startPos.x() - r, m_startPos.y() - r, r * 2, r * 2);
        }
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void Canvas::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        if (m_isDrawing) {
            m_isDrawing = false;
            QPointF endPos = snapPoint(event->scenePos());
            if (m_previewLine) { removeItem(m_previewLine); delete m_previewLine; m_previewLine = nullptr; }
            if (m_previewCircle) { removeItem(m_previewCircle); delete m_previewCircle; m_previewCircle = nullptr; }

            if (m_currentTool == Tool::Wall) emit lineFinished(Point(m_startPos.x(), m_startPos.y()), Point(endPos.x(), endPos.y()));
            else if (m_currentTool == Tool::Column) emit circleFinished(Point(m_startPos.x(), m_startPos.y()), QLineF(m_startPos, endPos).length());
        }
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void Canvas::wheelEvent(QGraphicsSceneWheelEvent* event) {
    if (event->modifiers() & Qt::ControlModifier) {
        if (views().isEmpty()) return;
        double factor = (event->delta() > 0) ? 1.15 : 1.0 / 1.15;
        views().first()->scale(factor, factor);
        event->accept();
    }
    else QGraphicsScene::wheelEvent(event);
}