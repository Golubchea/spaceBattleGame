#pragma once
#include <Visualization/Model/Shaders/Shader.hpp>
#include <glad/glad.h>
#include <memory>
#include <vector>
class Object3d {
public:
  Object3d() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
  }

    virtual void setupGeometry(const std::vector<float> &vertices,
                               const std::vector<unsigned int> &indices = {})
    {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(float),
                     vertices.data(),
                     GL_STATIC_DRAW);

        // Атрибуты позиции
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);

        // Атрибуты цвета
        glVertexAttribPointer(1,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              6 * sizeof(float),
                              (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        verticesSize = vertices.size();
    }

    virtual void renderEntity(const glm::mat4 &view,
                              const glm::mat4 &projection,
                              const glm::vec3 &cameraPos)
    {
        m_shader->use();
        glBindVertexArray(VAO);
        renderGeometry(view, projection);
        glBindVertexArray(0);
    }

    virtual void renderGeometry(const glm::mat4 &view, const glm::mat4 &projection) {}

    virtual ~Object3d() {
      glDeleteVertexArrays(1, &VAO);
      glDeleteBuffers(1, &VBO);
      glDeleteVertexArrays(1, &VAO);
      glDeleteBuffers(1, &VBO);
    }

    void setShader(const std::shared_ptr<Shader> &shader) { m_shader = shader; }
    std::shared_ptr<Shader> shader() const;

    glm::mat4 getPosition() const;
    void setPosition(const glm::mat4 &newPosition);

    unsigned int getVAO() const;

    unsigned int getVBO() const;

    unsigned int getVerticesSize() const;

protected:
    std::shared_ptr<Shader> m_shader;
    glm::mat4 position = glm::mat4(1.0f);
    unsigned int VAO, VBO;
    unsigned int verticesSize;
};

inline glm::mat4 Object3d::getPosition() const { return position; }

inline void Object3d::setPosition(const glm::mat4 &newPosition) {
  position = newPosition;
}

inline unsigned int Object3d::getVAO() const { return VAO; }

inline unsigned int Object3d::getVBO() const { return VBO; }

inline unsigned int Object3d::getVerticesSize() const { return verticesSize; }

inline std::shared_ptr<Shader> Object3d::shader() const { return m_shader; }
