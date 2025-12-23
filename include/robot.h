#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"

class Robot {
public:
    Robot();

    // GPU buffer management
    void initGPU();
    void destroyGPU();

    // Animation control
    void setBaseRotation(float deg);
    void raiseRightArm(float deltaDeg);

    // Head animation
    void setHeadYaw(float deg);
    void animateHead(float tSeconds);

    // Leg animation
    void animateLegs(float tSeconds);

    // Draw the robot
    void draw(Shader& shader);

private:
    // Cube geometry (reused for all parts)
    unsigned int cubeVAO, cubeVBO;

    // Joint rotation angles
    float baseRotationDeg;
    float rightArmDeg;
    float headYawDeg;
    float leftLegDeg;
    float rightLegDeg;

    // Draw one part of the robot (a cube)
    void drawCube(Shader& shader, const glm::mat4& parent,
                  const glm::vec3& scale, const glm::vec3& translate);
};