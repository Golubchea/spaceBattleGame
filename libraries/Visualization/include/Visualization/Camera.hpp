#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 2.0f, 9.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f))
        : position(position)
        , worldUp(up)
        , front(glm::vec3(0.0f, 0.0f, -1.0f))
        , yaw(-90.0f)
        , pitch(0.0f)
    {
        updateCameraVectors();
        updateProjection(800, 600);
    }

    glm::mat4 getViewMatrix() const { return glm::lookAt(position, position + front, up); }

    void processKeyboard(int key, float deltaTime, bool shifted)
    {
        float velocity = (shifted ? movementSpeedShifted : movementSpeed) * deltaTime;
        if (key == GLFW_KEY_W)
            position += front * velocity;
        if (key == GLFW_KEY_S)
            position -= front * velocity;
        if (key == GLFW_KEY_A)
            position -= glm::normalize(glm::cross(front, up)) * velocity;
        if (key == GLFW_KEY_D)
            position += glm::normalize(glm::cross(front, up)) * velocity;
    }

    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
    {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (constrainPitch) {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        updateCameraVectors();
    }

    glm::vec3 getPosition() const;

    glm::mat4 getProjection() const;

    void updateProjection(int width, int height)
    {
        projection = glm::perspective(fov, (float) width / (float) height, 0.1f, 500.0f);
    }

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 worldUp;
    glm::mat4 projection;
    float fov = glm::radians(45.0f);
    float yaw, pitch;
    float movementSpeed = 2.5f;
    float movementSpeedShifted = 20.5f;
    float mouseSensitivity = 0.1f;

    void updateCameraVectors()
    {
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(direction);

        // Вычисляем вектор "право" (right) как перпендикулярный front и worldUp
        glm::vec3 right = glm::normalize(glm::cross(front, worldUp));
        // Вычисляем новый вектор "вверх" (up) как перпендикулярный front и right
        up = glm::normalize(glm::cross(right, front));
    }
};

inline glm::vec3 Camera::getPosition() const
{
    return position;
}

inline glm::mat4 Camera::getProjection() const
{
    return projection;
}
