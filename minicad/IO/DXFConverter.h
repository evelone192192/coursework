#ifndef DXFCONVERTER_H
#define DXFCONVERTER_H

#include <string>
#include <vector>
#include <memory>
#include "../Core/Primitive.h"

class DXFConverter {
public:
    static bool saveToFile(const std::string& filename, const std::vector<std::unique_ptr<Primitive>>& primitives);

    static std::vector<std::unique_ptr<Primitive>> loadFromFile(const std::string& filename);
};

#endif