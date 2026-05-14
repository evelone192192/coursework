#include "Line.h"

Line::Line(int id, int layerId, Point start, Point end)
    : Primitive(id, layerId), m_start(start), m_end(end) {}

Primitive::Type Line::getType() const { return Primitive::Type::Line; }

std::string Line::getInfo() const { return "Line object"; }

void Line::move(double dx, double dy) {
    m_start.x += dx; m_start.y += dy;
    m_end.x += dx;   m_end.y += dy;
}

Point Line::start() const { return m_start; }
Point Line::end() const { return m_end; }