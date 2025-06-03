#pragma once
#include <Visualization/Model/Entity.hpp>
#include <Visualization/Model/Shaders/Shader.hpp>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <memory>
#include <vector>

class Grid : public Object3d
{
public:
    Grid(const std::shared_ptr<Shader> &shader)
    {
        const float gridSize = 10.0f; // Размер сетки
        const int steps = 20;         // Количество шагов по каждой оси
        const float stepSize = gridSize / steps;
        std::vector<float> gridVertices;
        for (int i = -steps; i <= steps; ++i) {
            // Линии вдоль оси X
            gridVertices.push_back(-gridSize);    // x
            gridVertices.push_back(0.0f);         // y
            gridVertices.push_back(i * stepSize); // z
            gridVertices.push_back(0.5f);         // r
            gridVertices.push_back(0.5f);         // g
            gridVertices.push_back(0.5f);         // b

            gridVertices.push_back(gridSize);     // x
            gridVertices.push_back(0.0f);         // y
            gridVertices.push_back(i * stepSize); // z
            gridVertices.push_back(0.5f);         // r
            gridVertices.push_back(0.5f);         // g
            gridVertices.push_back(0.5f);         // b

            // Линии вдоль оси Z
            gridVertices.push_back(i * stepSize); // x
            gridVertices.push_back(0.0f);         // y
            gridVertices.push_back(-gridSize);    // z
            gridVertices.push_back(0.5f);         // r
            gridVertices.push_back(0.5f);         // g
            gridVertices.push_back(0.5f);         // b

            gridVertices.push_back(i * stepSize); // x
            gridVertices.push_back(0.0f);         // y
            gridVertices.push_back(gridSize);     // z
            gridVertices.push_back(0.5f);         // r
            gridVertices.push_back(0.5f);         // g
            gridVertices.push_back(0.5f);         // b
        }
        setupGeometry(gridVertices);
        setShader(shader);
    }

    void renderGeometry(const glm::mat4 &view, const glm::mat4 &projection) override
    {
        // shader()->use();
        shader()->setMat4("model", position);
        shader()->setMat4("view", view);
        shader()->setMat4("projection", projection);
        glDrawArrays(GL_LINES, 0, getVerticesSize() / 6);
    }

private:
};
