#ifndef ADDPRIMITIVECOMMAND_H
#define ADDPRIMITIVECOMMAND_H
#include "Command.h"
#include "../Core/Primitive.h"
#include <vector>
#include <memory>

class AddPrimitiveCommand : public Command {
public:
    AddPrimitiveCommand(std::vector<std::unique_ptr<Primitive>>& storage, std::unique_ptr<Primitive> primitive);
    void execute() override;
    void undo() override;
private:
    std::vector<std::unique_ptr<Primitive>>& m_storage;
    std::unique_ptr<Primitive> m_primitive;
};
#endif