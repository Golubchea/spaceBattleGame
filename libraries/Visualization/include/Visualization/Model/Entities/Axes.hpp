#pragma once
#include <Visualization/Model/Entity.hpp>
#include <Visualization/Model/Shaders/Shader.hpp>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <memory>
#include <vector>

class Axes : public Object3d {
public:
    Axes(const std::shared_ptr<Shader> &shader)
    {
        constexpr const float width = 100.0f;
        std::vector<float> geometry = {// Ось X (красная)
                                       0.0f,
                                       0.0f,
                                       0.0f,
                                       width,
                                       0.0f,
                                       0.0f,
                                       width,
                                       0.0f,
                                       0.0f,
                                       1.0f,
                                       0.0f,
                                       0.0f,

                                       // Ось Y (зеленая)
                                       0.0f,
                                       0.0f,
                                       0.0f,
                                       0.0f,
                                       width,
                                       0.0f,
                                       0.0f,
                                       width,
                                       0.0f,
                                       0.0f,
                                       width,
                                       0.0f,

                                       // Ось Z (синяя)
                                       0.0f,
                                       0.0f,
                                       0.0f,
                                       0.0f,
                                       0.0f,
                                       width,
                                       0.0f,
                                       0.0f,
                                       width,
                                       0.0f,
                                       0.0f,
                                       width};

        setupGeometry(geometry);
        setShader(shader);
    }

    void renderGeometry(const glm::mat4 &view, const glm::mat4 &projection) override
    {
        //  shader()->use();
        shader()->setMat4("model", position);
        shader()->setMat4("view", view);
        shader()->setMat4("projection", projection);
        glDrawArrays(GL_LINES, 0, getVerticesSize() / 6);
    }

private:
};
