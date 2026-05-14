#include "CommandManager.h"

void CommandManager::executeCommand(std::unique_ptr<Command> cmd) {
    cmd->execute();
    m_undoStack.push(std::move(cmd));
    while (!m_redoStack.empty()) m_redoStack.pop();
}

void CommandManager::undo() {
    if (m_undoStack.empty()) return;
    auto cmd = std::move(m_undoStack.top());
    m_undoStack.pop();
    cmd->undo();
    m_redoStack.push(std::move(cmd));
}

void CommandManager::redo() {
    if (m_redoStack.empty()) return;
    auto cmd = std::move(m_redoStack.top());
    m_redoStack.pop();
    cmd->execute();
    m_undoStack.push(std::move(cmd));
}