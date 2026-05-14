#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H
#include "Command.h"
#include <stack>
#include <memory>

class CommandManager {
public:
    void executeCommand(std::unique_ptr<Command> cmd);
    void undo();
    void redo();
private:
    std::stack<std::unique_ptr<Command>> m_undoStack;
    std::stack<std::unique_ptr<Command>> m_redoStack;
};
#endif