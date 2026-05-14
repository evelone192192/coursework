#ifndef SELECTIONMANAGER_H
#define SELECTIONMANAGER_H

#include <set>

class SelectionManager {
public:
    SelectionManager();

    void select(int id, bool append = false);
    void deselect(int id);
    void clear();

    bool isSelected(int id) const;
    const std::set<int>& getSelectedIds() const;
    bool isEmpty() const;

private:
    std::set<int> m_selectedIds;
};

#endif