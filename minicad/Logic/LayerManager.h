#ifndef LAYERMANAGER_H
#define LAYERMANAGER_H

#include <string>
#include <map>

struct Layer {
    int id;
    std::string name;
    bool visible = true;
    int priority = 0;
};

class LayerManager {
public:
    LayerManager();

    int createLayer(const std::string& name);
    void deleteLayer(int id);
    Layer* getLayer(int id);

    int getActiveLayerId() const;
    void setActiveLayer(int id);

    void setLayerPriority(int id, int priority);
    std::map<int, int> getPriorityMap() const;

    const std::map<int, Layer>& getAllLayers() const;

private:
    std::map<int, Layer> m_layers;
    int m_activeLayerId;
    int m_nextLayerId;
};

#endif