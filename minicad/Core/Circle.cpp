#include "Circle.h"
#include <sstream>

Circle::Circle(int id, int layerId, Point center, double radius)
    : Primitive(id, layerId), m_center(center), m_radius(radius) {}

Primitive::Type Circle::getType() const { return Primitive::Type::Circle; }

std::string Circle::getInfo() const {
    std::stringstream ss;
    ss << "Circle (Center: " << m_center.x << ", " << m_center.y << ", Radius: " << m_radius << ")";
    return ss.str();
}