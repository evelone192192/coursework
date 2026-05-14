#ifndef PRIMITIVE_H
#define PRIMITIVE_H
#include <string>

struct Point {
    double x, y;
    Point(double _x = 0, double _y = 0) : x(_x), y(_y) {}
};

class Primitive {
public:
    enum class Type { Unknown, Line, Circle };
    Primitive(int id, int layerId);
    virtual ~Primitive() = default;
    virtual Type getType() const = 0;
    virtual std::string getInfo() const = 0;
    int getId() const;
protected:
    int m_id;
    int m_layerId;
};
#endif