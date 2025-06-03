#pragma once

#include <CommandLib/ICommand.hpp>
#include <vector>

class RecoveryMacroCommand : public ICommand {
public:
    void Add(std::shared_ptr<ICommand> cmd);
    void Execute(IObject* object) override;
    std::vector<uint8_t> Serialize() const override;
    virtual std::string Type() const override;

private:
    std::vector<std::shared_ptr<ICommand>> commands_;
};
