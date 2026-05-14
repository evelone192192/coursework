#include "Primitive.h"

Primitive::Primitive(int id, int layerId) : m_id(id), m_layerId(layerId) {}
int Primitive::getId() const { return m_id; }
int Primitive::getLayerId() const { return m_layerId; }
void Primitive::setLayerId(int id) { m_layerId = id; }
unsigned int Primitive::getColor() const { return m_color; }
void Primitive::setColor(unsigned int color) { m_color = color; }