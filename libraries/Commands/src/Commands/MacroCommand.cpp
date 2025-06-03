#include <CommandLib/Commands/MacroCommand.hpp>
#include <CommandLib/Exceptions/CommandException.hpp>

#include <sstream>

void MacroCommand::Add(std::shared_ptr<ICommand> command) {
    commands_.push_back(command);
}

void MacroCommand::Execute(IObject* object) {
    for (auto &cmd : commands_) {
        try {
            cmd->Execute(object);
        } catch (...) {
            // пробрасываем первое исключение
            throw CommandException("MacroCommand failed during execution");
        }
    }
}

std::vector<uint8_t> MacroCommand::Serialize() const {
    std::ostringstream oss;
    boost::archive::binary_oarchive oa(oss);
    oa << Type();
    oa << commands_;
    return {oss.str().begin(), oss.str().end()};
}

std::string MacroCommand::Type() const {
    return "MacroCommand";
}

std::vector<std::shared_ptr<ICommand>> MacroCommand::GetCommands() const {
    return commands_;
}
