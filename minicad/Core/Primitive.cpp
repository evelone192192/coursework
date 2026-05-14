#include "Primitive.h"
Primitive::Primitive(int id, int layerId) : m_id(id), m_layerId(layerId) {}
int Primitive::getId() const { return m_id; }