#pragma once
#include "glm/gtc/type_ptr.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
class Shader {
public:

    Shader(const char *vertexPath, const char *fragmentPath)
    {
        unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexPath);
        unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentPath);

        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);
        glLinkProgram(ID);

        checkCompileErrors(ID, "PROGRAM");

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        std::cout << "Shader program created with ID: " << ID << std::endl;
    }

    // Использование шейдерной программы
    void use() const {
        glUseProgram(ID);
    }

    // Установка uniform-переменных
    void setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }
    void setVec3(const std::string& name, const glm::vec3& vec) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
    }

    void setTexture(const std::string &name, unsigned int textureID, unsigned int unit) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(glGetUniformLocation(ID, name.c_str()), unit);
    }

    // Установка параметров материала
    void setMaterial(const std::string &name,
                     const glm::vec3 &ambient,
                     const glm::vec3 &diffuse,
                     const glm::vec3 &specular,
                     float shininess) const
    {
        setVec3(name + ".ambient", ambient);
        setVec3(name + ".diffuse", diffuse);
        setVec3(name + ".specular", specular);
        setFloat(name + ".shininess", shininess);
    }

    // Установка параметров света
    void setLight(const std::string &name,
                  const glm::vec3 &position,
                  const glm::vec3 &ambient,
                  const glm::vec3 &diffuse,
                  const glm::vec3 &specular) const
    {
        setVec3(name + ".position", position);
        setVec3(name + ".ambient", ambient);
        setVec3(name + ".diffuse", diffuse);
        setVec3(name + ".specular", specular);
    }

    // Деструктор: удаляем шейдерную программу
    ~Shader() {
        glDeleteProgram(ID);
    }

    unsigned int getID() const;

private:
    unsigned int ID; // ID шейдерной программы

    // Компиляция шейдера
    unsigned int compileShader(unsigned int type, const char* source) const {
        unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        // Проверяем успешность компиляции
        checkCompileErrors(shader, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");

        return shader;
    }

    // Проверка ошибок компиляции/линковки
    void checkCompileErrors(unsigned int shader, const std::string& type) const {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                throw std::runtime_error("ERROR::SHADER::" + type + "::COMPILATION_FAILED\n" + std::string(infoLog));
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                throw std::runtime_error("ERROR::SHADER::PROGRAM::LINKING_FAILED\n" + std::string(infoLog));
            }
        }
    }
};

inline unsigned int Shader::getID() const
{
    return ID;
}
