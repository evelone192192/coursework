#include "Line.h"
Line::Line(int id, int layerId, Point start, Point end)
    : Primitive(id, layerId), m_start(start), m_end(end) {}
Primitive::Type Line::getType() const { return Primitive::Type::Line; }
std::string Line::getInfo() const { return "Line object"; }