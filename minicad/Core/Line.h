#ifndef LINE_H
#define LINE_H
#include "Primitive.h"

class Line : public Primitive {
public:
    Line(int id, int layerId, Point start, Point end);
    Type getType() const override;
    std::string getInfo() const override;
    Point start() const { return m_start; }
    Point end() const { return m_end; }
private:
    Point m_start, m_end;
};
#endif