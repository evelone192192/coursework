#include "GeometryCalc.h"

double GeometryCalc::distance(Point p1, Point p2) {
    return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

bool GeometryCalc::isPointNearLine(Point p, Point start, Point end, double tolerance) {
    double d = distance(start, end);
    if (d == 0) return distance(p, start) < tolerance;

    double dist = std::abs((end.y - start.y) * p.x - (end.x - start.x) * p.y + end.x * start.y - end.y * start.x) / d;
    return dist < tolerance;
}

bool GeometryCalc::isPointInCircle(Point p, Point center, double radius) {
    return distance(p, center) <= radius;
}