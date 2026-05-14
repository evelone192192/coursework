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

    virtual void move(double dx, double dy) = 0;

    int getId() const;
    int getLayerId() const { return m_layerId; }
    void setLayerId(int id) { m_layerId = id; }

    unsigned int getColor() const { return m_color; }
    void setColor(unsigned int color) { m_color = color; }

protected:
    int m_id;
    int m_layerId;
    unsigned int m_color = 0xFF000000;
};
#endif