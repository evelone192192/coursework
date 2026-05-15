#ifndef CIRCLE_H
#define CIRCLE_H
#include "Primitive.h"

class Circle : public Primitive {
public:
    Circle(int id, int layerId, Point center, double radius);
    Type getType() const override;
    std::string getInfo() const override;

    void move(double dx, double dy) override;

    void setCenter(Point p);
    void setRadius(double r);

    Point center() const;
    double radius() const;
private:
    Point m_center;
    double m_radius;
};
#endif