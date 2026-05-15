#ifndef LINE_H
#define LINE_H
#include "Primitive.h"

class Line : public Primitive {
public:
    Line(int id, int layerId, Point start, Point end);
    Type getType() const override;
    std::string getInfo() const override;

    void move(double dx, double dy) override;

    void setStart(Point p);
    void setEnd(Point p);

    Point start() const;
    Point end() const;
private:
    Point m_start, m_end;
};
#endif