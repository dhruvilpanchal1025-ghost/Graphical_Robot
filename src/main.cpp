#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>

#include "Shader.h"
#include "camera.h"
#include "scene.h"
#include "robot.h"

// Global constants and objects
const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

Camera gCamera(glm::vec3(0.0f, 1.0f, 4.0f));
Scene  gScene;
Robot  gRobot;

float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool  firstMouse = true;

// Camera mode: 1 = free, 2 = orbit
int   gCameraMode = 1;

// Input processing
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float speed = 2.5f * 0.016f;

    // Free camera movement (mode 1)
    if (gCameraMode == 1) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) gCamera.processKeyboard(FORWARD,  speed);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) gCamera.processKeyboard(BACKWARD, speed);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) gCamera.processKeyboard(LEFT,     speed);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) gCamera.processKeyboard(RIGHT,    speed);
    }

    // Robot arm control
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    gRobot.raiseRightArm(+1.5f);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  gRobot.raiseRightArm(-1.5f);

    // Scene switching
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) gScene.setScene(1);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) gScene.setScene(2);
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) gScene.setScene(3);

    // Camera mode switching
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) gCameraMode = 1;
    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) gCameraMode = 2;
}

void mouse_callback(GLFWwindow*, double xpos, double ypos) {
    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }
    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;

    // Mouse look only in free camera mode
    if (gCameraMode == 1) {
        gCamera.processMouse(xoffset, yoffset);
    }
}

// Main program entry
int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_DEBUG, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        WIDTH, HEIGHT,
        "Robot Demo (Checkpoint 3)",
        nullptr, nullptr
    );
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetCursorPosCallback(window, mouse_callback);
    glEnable(GL_DEPTH_TEST);

    Shader shader("../resources/shaders/vertex_shader.glsl",
                  "../resources/shaders/fragment_shader.glsl");

    gScene.ensureGround();
    gScene.setScene(1);
    gRobot.initGPU();

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Get current time for animations
        float t = static_cast<float>(glfwGetTime());

        // Robot animations
        gRobot.animateHead(t);
        gRobot.animateLegs(t);

        // Set background color based on scene
        glm::vec3 cc = gScene.clearColor();
        glClearColor(cc.x, cc.y, cc.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        // Calculate View matrix based on camera mode
        glm::mat4 view;

        if (gCameraMode == 1) {
            // Free camera view
            view = gCamera.viewMatrix();
        } else {
            // Orbit camera calculation
            float orbitRadius = 4.0f;
            float orbitHeight = 1.6f;
            float orbitSpeed  = 0.4f;
            float angle       = orbitSpeed * t;

            glm::vec3 target(0.0f, 0.9f, 0.0f);
            glm::vec3 eye(
                target.x + orbitRadius * std::cos(angle),
                target.y + orbitHeight,
                target.z + orbitRadius * std::sin(angle)
            );

            view = glm::lookAt(eye, target, glm::vec3(0.0f, 1.0f, 0.0f));
        }

        // Projection matrix
        glm::mat4 proj = glm::perspective(glm::radians(45.0f),
                                          (float)WIDTH / (float)HEIGHT,
                                          0.1f, 100.0f);
        shader.setMat4("uView", view);
        shader.setMat4("uProj", proj);

        // Lighting setup
        shader.setInt("uUseLight", 1);
        shader.setVec3("uAmbient",    glm::vec3(0.18f, 0.18f, 0.18f));
        shader.setFloat("uShininess", 64.0f);
        shader.setVec3("uDirLightDir",   glm::normalize(glm::vec3(0.4f, 0.3f, 0.2f)));
        shader.setVec3("uDirLightColor", glm::vec3(1.0f, 0.65f, 0.25f));
        shader.setVec3("uPointPos",      glm::vec3(0.0f, 1.2f, 0.0f));
        shader.setVec3("uPointColor",    glm::vec3(0.2f, 0.6f, 1.0f));

        // Draw robot and scene
        gRobot.setBaseRotation(0.0f);

        gScene.draw(shader);
        gRobot.draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    gRobot.destroyGPU();
    glfwTerminate();
    return 0;
}