#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera {
public:
    // Initialize camera at a default position
    Camera(glm::vec3 position = glm::vec3(0.0f, 1.0f, 4.0f));

    // Get the view transformation matrix
    glm::mat4 viewMatrix() const;
    // Handle keyboard movement
    void processKeyboard(Camera_Movement direction, float delta);
    // Handle mouse rotation
    void processMouse(float xoffset, float yoffset);

private:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;

    // Recalculate camera vectors (Front, Right, Up)
    void updateCameraVectors();
};