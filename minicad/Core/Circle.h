#ifndef CIRCLE_H
#define CIRCLE_H
#include "Primitive.h"

class Circle : public Primitive {
public:
    Circle(int id, int layerId, Point center, double radius);
    Type getType() const override;
    std::string getInfo() const override;

    void move(double dx, double dy) override {
        m_center.x += dx; m_center.y += dy;
    }

    Point center() const { return m_center; }
    double radius() const { return m_radius; }
private:
    Point m_center;
    double m_radius;
};
#endif