#include <CommandLib/Commands/RetryCommand.hpp>
#include <iostream>

RetryCommand::RetryCommand(std::shared_ptr<ICommand> original) : original_(original) {}

void RetryCommand::Execute(IObject* object) {
    try {
        original_->Execute(object); // повторяем команду
    } catch (const std::exception& e) {
        throw; // если снова ошибка — пробрасываем
    }
}

std::vector<uint8_t> RetryCommand::Serialize() const
{
    return {};
}

std::string RetryCommand::Type() const
{
    return "RetryCommand";
}

std::shared_ptr<ICommand> RetryCommand::original() const
{
    return original_;
}
