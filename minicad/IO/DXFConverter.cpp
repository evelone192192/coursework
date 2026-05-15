#include "DXFConverter.h"
#include "../Core/Line.h"
#include "../Core/Circle.h"
#include <fstream>
#include <iostream>

bool DXFConverter::saveToFile(const std::string& filename, const std::vector<std::unique_ptr<Primitive>>& primitives) {
    std::ofstream f(filename);
    if (!f.is_open()) return false;

    f << "  0\nSECTION\n  2\nENTITIES\n";

    for (const auto& p : primitives) {
        if (p->getType() == Primitive::Type::Line) {
            Line* l = static_cast<Line*>(p.get());
            f << "  0\nLINE\n  8\n" << l->getLayerId() << "\n";
            f << " 10\n" << l->start().x << "\n 20\n" << l->start().y << "\n 30\n0.0\n";
            f << " 11\n" << l->end().x << "\n 21\n" << l->end().y << "\n 31\n0.0\n";
        }
        else if (p->getType() == Primitive::Type::Circle) {
            Circle* c = static_cast<Circle*>(p.get());
            f << "  0\nCIRCLE\n  8\n" << c->getLayerId() << "\n";
            f << " 10\n" << c->center().x << "\n 20\n" << c->center().y << "\n 30\n0.0\n";
            f << " 40\n" << c->radius() << "\n";
        }
    }

    f << "  0\nENDSEC\n  0\nEOF\n";
    f.close();
    return true;
}

std::vector<std::unique_ptr<Primitive>> DXFConverter::loadFromFile(const std::string& filename) {
    std::vector<std::unique_ptr<Primitive>> items;
    std::ifstream f(filename);
    if (!f.is_open()) return items;

    std::string line;
    int nextId = 9000;

    while (std::getline(f, line)) {
        line.erase(0, line.find_first_not_of(" "));

        if (line == "LINE") {
            double x1 = 0, y1 = 0, x2 = 0, y2 = 0;
            int layer = 0;
            while (std::getline(f, line) && line != "  0") {
                line.erase(0, line.find_first_not_of(" "));
                if (line == "10") { std::getline(f, line); x1 = std::stod(line); }
                else if (line == "20") { std::getline(f, line); y1 = std::stod(line); }
                else if (line == "11") { std::getline(f, line); x2 = std::stod(line); }
                else if (line == "21") { std::getline(f, line); y2 = std::stod(line); }
                else if (line == "8") { std::getline(f, line); layer = std::stoi(line); }
            }
            items.push_back(std::make_unique<Line>(nextId++, layer, Point(x1, y1), Point(x2, y2)));
        }
        else if (line == "CIRCLE") {
            double cx = 0, cy = 0, r = 0;
            int layer = 0;
            while (std::getline(f, line) && line != "  0") {
                line.erase(0, line.find_first_not_of(" "));
                if (line == "10") { std::getline(f, line); cx = std::stod(line); }
                else if (line == "20") { std::getline(f, line); cy = std::stod(line); }
                else if (line == "40") { std::getline(f, line); r = std::stod(line); }
                else if (line == "8") { std::getline(f, line); layer = std::stoi(line); }
            }
            items.push_back(std::make_unique<Circle>(nextId++, layer, Point(cx, cy), r));
        }
    }
    return items;
}