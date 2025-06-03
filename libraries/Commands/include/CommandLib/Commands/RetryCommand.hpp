#pragma once

#include <CommandLib/ICommand.hpp>

class RetryCommand : public ICommand {
public:
    explicit RetryCommand(std::shared_ptr<ICommand> original);
    void Execute(IObject* object) override;
    std::vector<uint8_t> Serialize() const override;
    std::string Type() const override;

    std::shared_ptr<ICommand> original() const;

private:
    std::shared_ptr<ICommand> original_;
};
