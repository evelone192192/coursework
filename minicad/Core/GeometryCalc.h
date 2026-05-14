#ifndef GEOMETRYCALC_H
#define GEOMETRYCALC_H

#include "Primitive.h"
#include <cmath>

class GeometryCalc {
public:
    static double distance(Point p1, Point p2);

    static bool isPointNearLine(Point p, Point start, Point end, double tolerance);
    static bool isPointInCircle(Point p, Point center, double radius);
};

#endif