#include <CommandLib/Commands/RecoveryMacroCommand.hpp>

void RecoveryMacroCommand::Add(std::shared_ptr<ICommand> cmd) {
    commands_.push_back(cmd);
}

void RecoveryMacroCommand::Execute(IObject* object) {
    for (auto& cmd : commands_) {
        try {
            cmd->Execute(object);
        } catch (...) {
            throw; // Пробрасываем дальше, пусть кто-то другой решает
        }
    }
}

std::vector<uint8_t> RecoveryMacroCommand::Serialize() const
{
    return {};
}

std::string RecoveryMacroCommand::Type() const
{
    return "RecoveryMacroCommand";
}
