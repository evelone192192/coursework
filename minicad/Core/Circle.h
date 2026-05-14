#ifndef CIRCLE_H
#define CIRCLE_H

#include "Primitive.h"

class Circle : public Primitive {
public:
    Circle(int id, int layerId, Point center, double radius);

    Type getType() const override { return Type::Circle; }
    std::string getInfo() const override;

    Point center() const { return m_center; }
    double radius() const { return m_radius; }

    void setParams(Point center, double radius) { m_center = center; m_radius = radius; }

private:
    Point m_center;
    double m_radius;
};

#endif