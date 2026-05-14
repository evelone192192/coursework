#include "SelectionManager.h"

SelectionManager::SelectionManager() {}

void SelectionManager::select(int id, bool append) {
    if (!append) {
        m_selectedIds.clear();
    }
    m_selectedIds.insert(id);
}

void SelectionManager::deselect(int id) {
    m_selectedIds.erase(id);
}

void SelectionManager::clear() {
    m_selectedIds.clear();
}

bool SelectionManager::isSelected(int id) const {
    return m_selectedIds.find(id) != m_selectedIds.end();
}

const std::set<int>& SelectionManager::getSelectedIds() const {
    return m_selectedIds;
}

bool SelectionManager::isEmpty() const {
    return m_selectedIds.empty();
}