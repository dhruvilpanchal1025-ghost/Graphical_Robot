#include "camera.h"

Camera::Camera(glm::vec3 position)
    : Position(position), Front(0.0f, 0.0f, -1.0f),
      WorldUp(0.0f, 1.0f, 0.0f), Yaw(-90.0f), Pitch(0.0f),
      MovementSpeed(2.5f), MouseSensitivity(0.1f)
{
    updateCameraVectors();
}

// Return the view matrix
glm::mat4 Camera::viewMatrix() const {
    return glm::lookAt(Position, Position + Front, Up);
}

// Process keyboard input for movement
void Camera::processKeyboard(Camera_Movement direction, float delta) {
    float velocity = MovementSpeed * delta;

    if (direction == FORWARD)  Position += Front * velocity;
    if (direction == BACKWARD) Position -= Front * velocity;
    if (direction == LEFT)     Position -= Right * velocity;
    if (direction == RIGHT)    Position += Right * velocity;
}

// Process mouse input for rotation
void Camera::processMouse(float xoffset, float yoffset) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    // Constrain pitch to avoid flipping
    if (Pitch > 89.0f) Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;

    updateCameraVectors();
}

// Recalculate Front, Right, and Up vectors
void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up    = glm::normalize(glm::cross(Right, Front));
}