#pragma once
#include <cmath> // Для M_PI
#include <iostream>

class Angle
{
public:
    // Конструктор по умолчанию
    Angle()
        : degrees_(0)
    {}

    // Конструктор с углом в градусах
    explicit Angle(int degrees)
        : degrees_(normalize(degrees))
    {}

    // Метод для получения угла в градусах
    int getDegrees() const { return degrees_; }

    // Метод для получения угла в радианах
    double getRadians() const { return degrees_ * M_PI / 180.0; }

    // Оператор сложения (возвращает новый объект)
    Angle operator+(const Angle &other) const { return Angle(degrees_ + other.degrees_); }

    // Оператор вычитания (возвращает новый объект)
    Angle operator-(const Angle &other) const { return Angle(degrees_ - other.degrees_); }

    // Вывод угла в поток
    friend std::ostream &operator<<(std::ostream &os, const Angle &angle)
    {
        os << "(" << angle.degrees_ << " deg., " << angle.getRadians() << " rad.)";
        return os;
    }

private:
    int degrees_; // Угол в градусах

    // Нормализация угла в диапазон [0, 360)
    static int normalize(int degrees)
    {
        degrees %= 360; // Приводим к диапазону [-359, 359]
        if (degrees < 0) {
            degrees += 360; // Приводим к диапазону [0, 360)
        }
        return degrees;
    }
};
