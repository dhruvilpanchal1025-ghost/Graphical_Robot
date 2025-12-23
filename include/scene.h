#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"

class Scene {
public:
    Scene();

    // Create shared ground geometry
    void ensureGround();

    // Set scene: 1=default, 2=space, 3=jungle
    void setScene(int s);

    // Draw active scene
    void draw(Shader& shader);

    // Get background color for current scene
    glm::vec3 clearColor() const;

private:
    int currentScene;

    // Ground geometry (reused)
    unsigned int groundVAO;
    unsigned int groundVBO;

    // Stars geometry (for space scene)
    unsigned int starVAO;
    unsigned int starVBO;
    int          starCount;

    void ensureStars();

    void drawGroundScene(Shader& shader);
    void drawSpaceScene(Shader& shader);
    void drawJungleScene(Shader& shader);
};