#include <CommandLib/Commands/CheckFuelCommand.hpp>
#include <CommandLib/Commands/LogExceptionCommand.hpp>
#include <CommandLib/Commands/RetryCommand.hpp>
#include <CommandLib/Exceptions/ExceptionHandler.hpp>
#include <CommandLib/Exceptions/LogRecoveryHandler.hpp>
#include <CommandLib/Exceptions/NotEnoughFuelException.hpp>
#include <CommandLib/Exceptions/RetryRecoveryHandler.hpp>
#include <CommandLib/Exceptions/TwoTriesThenLogHandler.hpp>
#include <CommandLib/Mocks.hpp>
#include <gtest/gtest.h>

TEST(ExceptionHandlerTest, RetryOnceThenLog) {
    ExceptionHandler::GetInstance().RegisterHandler(std::make_shared<LogRecoveryHandler>());
    ExceptionHandler::GetInstance().RegisterHandler(std::make_shared<RetryRecoveryHandler>());

    MockIObject obj;
    float initial_fuel = 0.5f;
    std::vector<uint8_t> fuel_data(reinterpret_cast<const uint8_t *>(&initial_fuel),
                                   reinterpret_cast<const uint8_t *>(&initial_fuel) + sizeof(float));
    obj.SetProperty("fuel", fuel_data);

    auto fuelCmd = std::make_shared<CheckFuelCommand>();
    EXPECT_THROW(fuelCmd->Execute(&obj), NotEnoughFuelException);

    try {
        fuelCmd->Execute(&obj);
    } catch (const std::exception& e) {
        auto recovery = ExceptionHandler::GetInstance().Handle(fuelCmd, e);
        ASSERT_TRUE(recovery != nullptr);
        recovery->Execute(&obj); // повтор
    }
}

TEST(TwoTriesThenLogTest, FailsAfterTwoRetries)
{
    // Регистрируем обработчики
    ExceptionHandler::GetInstance().RegisterHandler(std::make_shared<TwoTriesThenLogHandler>());
    ExceptionHandler::GetInstance().RegisterHandler(std::make_shared<LogRecoveryHandler>());

    MockIObject obj;

    // fuelCmd теперь ICommand*
    auto fuelCmd = std::shared_ptr<ICommand>(std::make_shared<CheckFuelCommand>());

    for (int i = 0; i < 3; ++i) {
        try {
            fuelCmd->Execute(&obj);
        } catch (const std::exception &e) {
            auto recovery = ExceptionHandler::GetInstance().Handle(fuelCmd, e);
            if (!recovery)
                break;

            // Пробуем понять, что вернулось: RetryCommand или LogExceptionCommand
            if (auto retry = std::dynamic_pointer_cast<RetryCommand>(recovery)) {
                fuelCmd = retry->original(); // повторяем оригинальную команду
            } else {
                fuelCmd = recovery; // это, например, LogExceptionCommand
            }
        }
    }

    // На третьей итерации должна быть команда логирования
    EXPECT_EQ(typeid(*fuelCmd.get()), typeid(LogExceptionCommand));
}
