#ifndef LAYERMANAGER_H
#define LAYERMANAGER_H
#include <string>
#include <map>

struct Layer {
    int id;
    std::string name;
};

class LayerManager {
public:
    LayerManager();
    int createLayer(const std::string& name);
    void deleteLayer(int id);
    Layer* getLayer(int id);
private:
    std::map<int, Layer> m_layers;
    int m_nextLayerId;
};
#endif