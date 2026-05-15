#include "Line.h"
#include <sstream>
#include <iomanip>

Line::Line(int id, int layerId, Point start, Point end)
    : Primitive(id, layerId), m_start(start), m_end(end) {}

Primitive::Type Line::getType() const { return Primitive::Type::Line; }

std::string Line::getInfo() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1);
    ss << "S:(" << m_start.x << "," << m_start.y << ") E:(" << m_end.x << "," << m_end.y << ")";
    return ss.str();
}

void Line::move(double dx, double dy) {
    m_start.x += dx; m_start.y += dy;
    m_end.x += dx;   m_end.y += dy;
}

void Line::setStart(Point p) { m_start = p; }
void Line::setEnd(Point p) { m_end = p; }
Point Line::start() const { return m_start; }
Point Line::end() const { return m_end; }