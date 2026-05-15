#include "Circle.h"
#include <sstream>
#include <iomanip>

Circle::Circle(int id, int layerId, Point center, double radius)
    : Primitive(id, layerId), m_center(center), m_radius(radius) {}

Primitive::Type Circle::getType() const { return Primitive::Type::Circle; }

std::string Circle::getInfo() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1);
    ss << "C:(" << m_center.x << "," << m_center.y << ") R:" << m_radius;
    return ss.str();
}

void Circle::move(double dx, double dy) {
    m_center.x += dx; m_center.y += dy;
}

void Circle::setCenter(Point p) { m_center = p; }
void Circle::setRadius(double r) { m_radius = r; }
Point Circle::center() const { return m_center; }
double Circle::radius() const { return m_radius; }