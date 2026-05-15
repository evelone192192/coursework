#include "Primitive.h"
#include "Line.h" 
#include "Circle.h"

Primitive::Primitive(int id, int layerId) : m_id(id), m_layerId(layerId) {}

std::unique_ptr<Primitive> Primitive::create(Type type, int id, int layerId, Point p1, Point p2, double r) {
    if (type == Type::Line) {
        return std::make_unique<Line>(id, layerId, p1, p2);
    }
    else if (type == Type::Circle) {
        return std::make_unique<Circle>(id, layerId, p1, r);
    }
    return nullptr;
}

int Primitive::getId() const { return m_id; }
int Primitive::getLayerId() const { return m_layerId; }
void Primitive::setLayerId(int id) { m_layerId = id; }
unsigned int Primitive::getColor() const { return m_color; }
void Primitive::setColor(unsigned int color) { m_color = color; }