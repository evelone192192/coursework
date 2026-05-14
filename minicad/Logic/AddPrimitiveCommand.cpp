#include "AddPrimitiveCommand.h"

AddPrimitiveCommand::AddPrimitiveCommand(std::vector<std::unique_ptr<Primitive>>& storage, std::unique_ptr<Primitive> primitive)
    : m_storage(storage), m_primitive(std::move(primitive)) {}

void AddPrimitiveCommand::execute() {
    if (m_primitive) m_storage.push_back(std::move(m_primitive));
}

void AddPrimitiveCommand::undo() {
    if (!m_storage.empty()) {
        m_primitive = std::move(m_storage.back());
        m_storage.pop_back();
    }
}