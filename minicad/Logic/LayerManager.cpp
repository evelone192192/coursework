#include "LayerManager.h"

LayerManager::LayerManager() : m_activeLayerId(0), m_nextLayerId(0) {
    createLayer("Default");
}

int LayerManager::createLayer(const std::string& name) {
    int id = m_nextLayerId++;
    m_layers[id] = { id, name, true, id };
    return id;
}

void LayerManager::deleteLayer(int id) {
    if (m_layers.size() > 1) m_layers.erase(id);
}

Layer* LayerManager::getLayer(int id) {
    if (m_layers.count(id)) return &m_layers[id];
    return nullptr;
}

int LayerManager::getActiveLayerId() const {
    return m_activeLayerId;
}

void LayerManager::setActiveLayer(int id) {
    if (m_layers.count(id)) m_activeLayerId = id;
}

void LayerManager::setLayerPriority(int id, int priority) {
    if (m_layers.count(id)) m_layers[id].priority = priority;
}

std::map<int, int> LayerManager::getPriorityMap() const {
    std::map<int, int> pMap;
    for (auto const& [id, layer] : m_layers) {
        pMap[id] = layer.priority;
    }
    return pMap;
}

const std::map<int, Layer>& LayerManager::getAllLayers() const {
    return m_layers;
}