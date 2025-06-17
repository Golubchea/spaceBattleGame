#pragma once
#include <array>
#include <iostream>

template<typename T, std::size_t N>
class Vector
{
public:
    // Конструктор по умолчанию
    Vector() = default;

    // Конструктор, принимающий список инициализации
    Vector(std::initializer_list<T> values)
    {
        std::size_t i = 0;
        for (const auto &value : values) {
            if (i < N) {
                data[i] = value;
                ++i;
            }
        }
    }

    Vector<T, N> operator+(const Vector<T, N> &other) const
    {
        Vector<T, N> result;
        for (std::size_t i = 0; i < N; ++i) {
            result.data[i] = data[i] + other.data[i];
        }
        return result;
    }

    // Оператор доступа по индексу
    T &operator[](std::size_t index) { return data[index]; }
    const T &operator[](std::size_t index) const { return data[index]; }

    // Скалярное произведение (dot)
    T dot(const Vector &other) const
    {
        T result = 0;
        for (std::size_t i = 0; i < N; ++i) {
            result += data[i] * other.data[i];
        }
        return result;
    }

    // Векторное произведение (cross)
    Vector cross(const Vector &other) const
    {
        static_assert(N == 3, "Cross product is only defined for 3D vectors");
        return Vector({data[1] * other.data[2] - data[2] * other.data[1],
                       data[2] * other.data[0] - data[0] * other.data[2],
                       data[0] * other.data[1] - data[1] * other.data[0]});
    }

    // Перегрузка оператора вывода
    friend std::ostream &operator<<(std::ostream &os, const Vector &vec)
    {
        os << "(";
        for (std::size_t i = 0; i < N; ++i) {
            os << vec.data[i];
            if (i < N - 1)
                os << ", ";
        }
        os << ")";
        return os;
    }

    T &x() { return data[0]; }

    const T &x() const { return data[0]; }

    T &y() { return data[1]; }

    const T &y() const { return data[1]; }

    T &z() { return data[2]; }

    const T &z() const { return data[2]; }

private:
    std::array<T, N> data{};
};

// Специализация для 2D вектора (векторное произведение возвращает скаляр)
template<typename T>
class Vector<T, 2>
{
public:
    Vector() = default;

    Vector(std::initializer_list<T> values)
    {
        std::size_t i = 0;
        for (const auto &value : values) {
            if (i < 2) {
                data[i] = value;
                ++i;
            }
        }
    }

    Vector<T, 2> operator+(const Vector<T, 2> &other) const
    {
        Vector<T, 2> result;
        for (std::size_t i = 0; i < 2; ++i) {
            result.data[i] = data[i] + other.data[i];
        }
        return result;
    }

    T &operator[](std::size_t index) { return data[index]; }
    const T &operator[](std::size_t index) const { return data[index]; }

    // Скалярное произведение (dot)
    T dot(const Vector &other) const { return data[0] * other.data[0] + data[1] * other.data[1]; }

    // Векторное произведение (cross) для 2D векторов
    T cross(const Vector &other) const { return data[0] * other.data[1] - data[1] * other.data[0]; }

    friend std::ostream &operator<<(std::ostream &os, const Vector &vec)
    {
        os << "(" << vec.data[0] << ", " << vec.data[1] << ")";
        return os;
    }

    // Методы доступа к x и y (только для 2D векторов)
    T &x() { return data[0]; }

    const T &x() const { return data[0]; }

    T &y() { return data[1]; }

    const T &y() const { return data[1]; }

private:
    std::array<T, 2> data{};
};
