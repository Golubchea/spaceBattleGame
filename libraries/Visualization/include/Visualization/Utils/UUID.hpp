#include <iostream>
#include <string>
#include <random>
#include <sstream>
#include <iomanip>

class UUID {
public:
    static std::string generate() {
        std::random_device rd; // Генератор случайных чисел
        std::mt19937 gen(rd()); // Инициализация генератора
        std::uniform_int_distribution<> dist(0, 15); // Диапазон [0, 15]

        // Функция для преобразования числа в шестнадцатеричный символ
        auto toHex = [](int value) -> char {
            return "0123456789abcdef"[value];
        };

        // Генерация частей UUID
        std::stringstream ss;
        for (int i = 0; i < 8; ++i) ss << toHex(dist(gen)); // Первые 8 символов
        ss << "-";
        for (int i = 0; i < 4; ++i) ss << toHex(dist(gen)); // Следующие 4 символа
        ss << "-4"; // Версия 4
        for (int i = 0; i < 3; ++i) ss << toHex(dist(gen)); // Еще 3 символа
        ss << "-";
        ss << toHex(dist(gen) & 0x3 | 0x8); // Вариант UUID (8, 9, a, b)
        for (int i = 0; i < 3; ++i) ss << toHex(dist(gen)); // Еще 3 символа
        ss << "-";
        for (int i = 0; i < 12; ++i) ss << toHex(dist(gen)); // Последние 12 символов

        return ss.str();
    }
};