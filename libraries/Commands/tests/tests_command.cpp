#include "CommandLib/Commands/RotateAndChangeVelocityCommand.hpp"
#include <CommandLib/Commands/BurnFuelCommand.hpp>

#include <CommandLib/Commands/MacroCommand.hpp>
#include <CommandLib/Commands/MoveCommand.hpp>
#include <CommandLib/Commands/MoveWithFuelCommand.hpp>
#include <CommandLib/Commands/RotateCommand.hpp>
#include <CommandLib/Exceptions/CommandException.hpp>
#include <CommandLib/Mocks.hpp>
#include <gtest/gtest.h>

TEST(CommandTest, MoveCommand_Execute_UpdatesPosition) {
    MockIObject obj;
    std::array<double, 2> initial_pos{0.0, 0.0};
    std::vector<uint8_t> pos_data(reinterpret_cast<const uint8_t*>(initial_pos.data()),
                                  reinterpret_cast<const uint8_t*>(initial_pos.data()) + sizeof(double) * 2);
    obj.SetProperty("position", pos_data);

    MoveCommand cmd({1.0f, 2.0f});
    cmd.Execute(&obj);

    auto result = obj.GetProperty("position");
    ASSERT_EQ(result.size(), sizeof(double) * 2);

    double* data = reinterpret_cast<double*>(result.data());
    EXPECT_DOUBLE_EQ(data[0], 1.0);
    EXPECT_DOUBLE_EQ(data[1], 2.0);
}

TEST(CommandTest, RotateCommand_Execute_UpdatesRotation) {
    MockIObject obj;
    float initial_angle = 0.0f;
    std::vector<uint8_t> angle_data(reinterpret_cast<const uint8_t*>(&initial_angle),
                                    reinterpret_cast<const uint8_t*>(&initial_angle) + sizeof(float));
    obj.SetProperty("rotation", angle_data);

    RotateCommand cmd(45.0f);
    cmd.Execute(&obj);

    auto result = obj.GetProperty("rotation");
    ASSERT_EQ(result.size(), sizeof(float));

    float* data = reinterpret_cast<float*>(result.data());
    EXPECT_FLOAT_EQ(*data, 45.0f);
}

TEST(CommandTest, BurnFuelCommand_Execute_DecreasesFuel) {
    MockIObject obj;
    float initial_fuel = 100.0f;
    std::vector<uint8_t> fuel_data(reinterpret_cast<const uint8_t*>(&initial_fuel),
                                   reinterpret_cast<const uint8_t*>(&initial_fuel) + sizeof(float));
    obj.SetProperty("fuel", fuel_data);

    BurnFuelCommand cmd(30.0f);
    cmd.Execute(&obj);

    auto result = obj.GetProperty("fuel");
    ASSERT_EQ(result.size(), sizeof(float));

    float* data = reinterpret_cast<float*>(result.data());
    EXPECT_FLOAT_EQ(*data, 70.0f);
}

TEST(CommandTest, CheckFuelCommand_ThrowsIfNoFuelComponent)
{
    MockIObject obj;
    EXPECT_THROW(CheckFuelCommand().Execute(&obj), CommandException);
}

TEST(CommandTest, CheckFuelCommand_PassIfEnoughFuel)
{
    MockIObject obj;
    float fuel = 10.0f;
    obj.SetProperty("fuel",
                    std::vector<uint8_t>((uint8_t *) &fuel, (uint8_t *) &fuel + sizeof(fuel)));

    EXPECT_NO_THROW(CheckFuelCommand().Execute(&obj));
}

TEST(CommandTest, MacroCommand_Execute_RunsAllCommands) {
    MockIObject obj;

    // Установим начальную позицию
    std::array<double, 2> initial_pos{0.0, 0.0};
    std::vector<uint8_t> pos_data(reinterpret_cast<const uint8_t*>(initial_pos.data()),
                                  reinterpret_cast<const uint8_t*>(initial_pos.data()) + sizeof(double) * 2);
    obj.SetProperty("position", pos_data);

    // Создаем и добавляем команды в макрокоманду
    MacroCommand macro;

    std::array<double, 2> p1 = {1.0, 2.0};
    std::array<double, 2> p2 = {3.0, 4.0};
    macro.Add(std::make_shared<MoveCommand>(p1));
    macro.Add(std::make_shared<MoveCommand>(p2));

    // Выполняем макрокоманду
    macro.Execute(&obj);

    auto result = obj.GetProperty("position");
    double* data = reinterpret_cast<double*>(result.data());
    EXPECT_DOUBLE_EQ(data[0], 4.0);
    EXPECT_DOUBLE_EQ(data[1], 6.0);
}

TEST(CommandTest, MoveCommand_Serialize_Deserialize_Execute) {
    MoveCommand original({1.0f, 2.0f});
    auto data = original.Serialize();

    auto deserialized = ICommand::Deserialize(data.data(), data.size(), nullptr);
    MockIObject obj;

    std::array<double, 2> initial_pos{0.0, 0.0};
    std::vector<uint8_t> pos_data(reinterpret_cast<const uint8_t*>(initial_pos.data()),
                                  reinterpret_cast<const uint8_t*>(initial_pos.data()) + sizeof(double) * 2);
    obj.SetProperty("position", pos_data);

    deserialized->Execute(&obj);

    auto result = obj.GetProperty("position");
    double* data_out = reinterpret_cast<double*>(result.data());
    EXPECT_DOUBLE_EQ(data_out[0], 1.0);
    EXPECT_DOUBLE_EQ(data_out[1], 2.0);
}

TEST(CommandTest, MoveWithFuelCommand_SuccessfulMove)
{
    MockIObject obj;

    float initial_fuel = 5.0f;
    obj.SetProperty("fuel",
                    std::vector<uint8_t>((uint8_t *) &initial_fuel,
                                         (uint8_t *) (&initial_fuel + 1)));

    std::array<double, 2> pos{0.0, 0.0};
    obj.SetProperty("position",
                    std::vector<uint8_t>((uint8_t *) pos.data(),
                                         (uint8_t *) pos.data() + sizeof(pos)));

    MoveWithFuelCommand cmd({1.0, 0.0});
    cmd.Execute(&obj);

    auto result_pos = obj.GetProperty("position");
    double *data = reinterpret_cast<double *>(result_pos.data());
    EXPECT_DOUBLE_EQ(data[0], 1.0);
    EXPECT_DOUBLE_EQ(data[1], 0.0);

    auto result_fuel = obj.GetProperty("fuel");
    float *fuel = reinterpret_cast<float *>(result_fuel.data());
    EXPECT_FLOAT_EQ(*fuel, 4.0f);
}

TEST(CommandTest, MacroCommand_ThrowsIfAnyFails)
{
    MockIObject obj;

    MacroCommand macro;
    macro.Add(std::make_shared<CheckFuelCommand>());
    macro.Add(std::make_shared<MoveCommand>(std::array<double, 2>{1.0, 0.0}));

    EXPECT_THROW(macro.Execute(&obj), CommandException); // топлива нет → ошибка
}

TEST(CommandTest, RotateAndChangeVelocityCommand_ChangeOnlyRotation)
{
    MockIObject obj;

    float initial_angle = 0.0f;
    obj.SetProperty("rotation",
                    std::vector<uint8_t>(reinterpret_cast<const uint8_t *>(&initial_angle),
                                         reinterpret_cast<const uint8_t *>(&initial_angle + 1)));

    RotateAndChangeVelocityCommand cmd(45.0f, {0.0, 0.0});
    cmd.Execute(&obj);

    auto result_angle = obj.GetProperty("rotation");
    float *angle = reinterpret_cast<float *>(result_angle.data());
    EXPECT_NEAR(*angle, 45.0f, 0.01f);

    EXPECT_FALSE(obj.HasProperty("velocity")); // скорость не должна измениться
}

TEST(CommandTest, RotateAndChangeVelocityCommand_ChangeRotationAndVelocity)
{
    MockIObject obj;

    float initial_angle = 0.0f;
    obj.SetProperty("rotation",
                    std::vector<uint8_t>(reinterpret_cast<const uint8_t *>(&initial_angle),
                                         reinterpret_cast<const uint8_t *>(&initial_angle + 1)));

    std::array<double, 2> initial_vel{1.0, 0.0};
    obj.SetProperty("velocity",
                    std::vector<uint8_t>(reinterpret_cast<const uint8_t *>(initial_vel.data()),
                                         reinterpret_cast<const uint8_t *>(initial_vel.data())
                                             + 2 * sizeof(double)));

    RotateAndChangeVelocityCommand cmd(90.0f, {0.0, 1.0});
    cmd.Execute(&obj);

    auto result_angle = obj.GetProperty("rotation");
    float *angle = reinterpret_cast<float *>(result_angle.data());
    EXPECT_NEAR(*angle, 90.0f, 0.01f);

    auto result_vel = obj.GetProperty("velocity");
    double *velocity = reinterpret_cast<double *>(result_vel.data());
    EXPECT_DOUBLE_EQ(velocity[0], 1.0);
    EXPECT_DOUBLE_EQ(velocity[1], 1.0);
}
