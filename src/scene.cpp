#include "scene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>

Scene::Scene()
    : currentScene(1),
      groundVAO(0), groundVBO(0),
      starVAO(0), starVBO(0), starCount(0) {}

// Set the current scene index (1, 2, or 3)
void Scene::setScene(int s) {
    if (s < 1) s = 1;
    if (s > 3) s = 3;
    currentScene = s;
}

// Initialize ground geometry
void Scene::ensureGround() {
    if (groundVAO) return;

    // positions + normals
    struct PN { float x,y,z, nx,ny,nz; };
    PN verts[] = {
        {-5,0,-5, 0,1,0}, { 5,0,-5, 0,1,0}, { 5,0, 5, 0,1,0},
        {-5,0,-5, 0,1,0}, { 5,0, 5, 0,1,0}, {-5,0, 5, 0,1,0}
    };

    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);

    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PN), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PN), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

// Initialize star geometry (for space scene)
void Scene::ensureStars() {
    if (starVAO) return;

    std::vector<float> data; // x,y,z,nx,ny,nz
    const int N = 80;
    data.reserve(N * 6);

    for (int i = 0; i < N; ++i) {
        float angle  = (float)i * 0.4f;
        float radius = 12.0f + (i % 5);
        float height = 4.0f + (i % 7) * 0.4f;

        float x = std::cos(angle) * radius;
        float z = std::sin(angle) * radius;
        float y = height;

        data.push_back(x);
        data.push_back(y);
        data.push_back(z);
        data.push_back(0.0f);
        data.push_back(0.0f);
        data.push_back(-1.0f);
    }

    starCount = N;

    glGenVertexArrays(1, &starVAO);
    glGenBuffers(1, &starVBO);

    glBindVertexArray(starVAO);
    glBindBuffer(GL_ARRAY_BUFFER, starVBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

// Get background color based on scene
glm::vec3 Scene::clearColor() const {
    if (currentScene == 1) {
        return glm::vec3(0.45f, 0.70f, 0.95f);
    } else if (currentScene == 2) {
        return glm::vec3(0.02f, 0.02f, 0.08f);
    } else {
        return glm::vec3(0.10f, 0.25f, 0.12f);
    }
}

// Draw the current scene
void Scene::draw(Shader& shader) {
    ensureGround();

    if (currentScene == 1)
        drawGroundScene(shader);
    else if (currentScene == 2)
        drawSpaceScene(shader);
    else
        drawJungleScene(shader);
}

// Scene 1: Default ground
void Scene::drawGroundScene(Shader& shader) {
    shader.setVec3("uBaseColor", glm::vec3(0.20f, 0.60f, 0.80f));
    glm::mat4 model(1.0f);
    shader.setMat4("uModel", model);

    glBindVertexArray(groundVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

// Scene 2: Space platform + stars
void Scene::drawSpaceScene(Shader& shader) {
    ensureStars();

    // Platform
    shader.setVec3("uBaseColor", glm::vec3(0.20f, 0.20f, 0.28f));
    glm::mat4 platform(1.0f);
    platform = glm::translate(platform, glm::vec3(0.0f, -0.3f, 0.0f));
    platform = glm::scale(platform, glm::vec3(1.8f, 0.05f, 1.8f));
    shader.setMat4("uModel", platform);

    glBindVertexArray(groundVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Stars
    shader.setVec3("uBaseColor", glm::vec3(1.0f, 1.0f, 1.0f));
    glm::mat4 starModel(1.0f);
    shader.setMat4("uModel", starModel);

    glBindVertexArray(starVAO);
    glPointSize(3.0f);
    glDrawArrays(GL_POINTS, 0, starCount);
    glBindVertexArray(0);
}

// Scene 3: Jungle ground + small bushes
void Scene::drawJungleScene(Shader& shader) {
    glBindVertexArray(groundVAO);

    // Ground
    shader.setVec3("uBaseColor", glm::vec3(0.20f, 0.75f, 0.20f));
    glm::mat4 ground = glm::mat4(1.0f);
    shader.setMat4("uModel", ground);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Bushes

    shader.setVec3("uBaseColor", glm::vec3(0.10f, 0.50f, 0.15f));

    // Bush 1
    {
        glm::mat4 b1 = glm::mat4(1.0f);
        b1 = glm::translate(b1, glm::vec3(1.5f, 0.02f, 1.0f));
        b1 = glm::scale(b1, glm::vec3(0.3f, 1.0f, 0.2f));
        shader.setMat4("uModel", b1);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // Bush 2
    {
        glm::mat4 b2 = glm::mat4(1.0f);
        b2 = glm::translate(b2, glm::vec3(-1.2f, 0.02f, -1.0f));
        b2 = glm::scale(b2, glm::vec3(0.25f, 1.0f, 0.25f));
        shader.setMat4("uModel", b2);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(0);
}