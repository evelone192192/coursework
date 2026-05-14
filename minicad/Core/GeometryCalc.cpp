#include "GeometryCalc.h"
#include <algorithm>
#include <cmath>

double GeometryCalc::distance(Point p1, Point p2) {
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

bool GeometryCalc::isPointNearLine(Point p, Point start, Point end, double tolerance) {
    double l2 = std::pow(distance(start, end), 2);
    if (l2 == 0.0) return distance(p, start) < tolerance;

    double t = ((p.x - start.x) * (end.x - start.x) + (p.y - start.y) * (end.y - start.y)) / l2;
    t = std::max(0.0, std::min(1.0, t));

    Point projection(start.x + t * (end.x - start.x),
        start.y + t * (end.y - start.y));

    return distance(p, projection) < tolerance;
}

bool GeometryCalc::isPointInCircle(Point p, Point center, double radius) {
    double dist = distance(p, center);
    return std::abs(dist - radius) < 8.0;
}