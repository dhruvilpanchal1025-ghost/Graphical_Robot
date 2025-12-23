#include "robot.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>

Robot::Robot()
: cubeVAO(0), cubeVBO(0),
  baseRotationDeg(0.0f),
  rightArmDeg(0.0f),
  headYawDeg(0.0f),
  leftLegDeg(0.0f),
  rightLegDeg(0.0f) {}

void Robot::setBaseRotation(float deg) { baseRotationDeg = deg; }
void Robot::raiseRightArm(float d) { rightArmDeg = glm::clamp(rightArmDeg + d, -10.0f, 90.0f); }

// Set head rotation
void Robot::setHeadYaw(float deg) {
    headYawDeg = deg;
}

// Time-based head movement
void Robot::animateHead(float tSeconds) {
    const float maxAngle = 25.0f;
    const float speed    = 2.0f;
    headYawDeg = maxAngle * std::sin(speed * tSeconds);
}

// Time-based leg movement (walking in place)
void Robot::animateLegs(float tSeconds) {
    const float stepAngle = 25.0f;
    const float speed     = 3.0f;
    float s = std::sin(speed * tSeconds);

    leftLegDeg  =  stepAngle * s;
    rightLegDeg = -stepAngle * s;
}

// Initialize cube geometry buffers
void Robot::initGPU() {
    if (cubeVAO) return;
    struct Vertex { float x,y,z; float nx,ny,nz; };
    const float h = 0.5f;
    const Vertex vertices[] = {
        // cube vertices with normals for each face
        // +X
        { h,-h,-h,1,0,0},{ h, h,-h,1,0,0},{ h, h, h,1,0,0},
        { h,-h,-h,1,0,0},{ h, h, h,1,0,0},{ h,-h, h,1,0,0},
        // -X
        {-h,-h,-h,-1,0,0},{-h,-h, h,-1,0,0},{-h, h, h,-1,0,0},
        {-h,-h,-h,-1,0,0},{-h, h, h,-1,0,0},{-h, h,-h,-1,0,0},
        // +Y
        {-h, h,-h,0,1,0},{-h, h, h,0,1,0},{ h, h, h,0,1,0},
        {-h, h,-h,0,1,0},{ h, h, h,0,1,0},{ h, h,-h,0,1,0},
        // -Y
        {-h,-h,-h,0,-1,0},{ h,-h,-h,0,-1,0},{ h,-h, h,0,-1,0},
        {-h,-h,-h,0,-1,0},{ h,-h, h,0,-1,0},{ h,-h, h,0,-1,0},
        // +Z
        {-h,-h, h,0,0,1},{ h,-h, h,0,0,1},{ h, h, h,0,0,1},
        {-h,-h, h,0,0,1},{ h, h, h,0,0,1},{-h, h, h,0,0,1},
        // -Z
        {-h,-h,-h,0,0,-1},{-h, h,-h,0,0,-1},{ h, h,-h,0,0,-1},
        {-h,-h,-h,0,0,-1},{ h, h,-h,0,0,-1},{ h,-h,-h,0,0,-1}
    };

    // set up VAO/VBO for the cube
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

// Cleanup GPU buffers
void Robot::destroyGPU() {
    if (cubeVBO) glDeleteBuffers(1, &cubeVBO);
    if (cubeVAO) glDeleteVertexArrays(1, &cubeVAO);
    cubeVBO = cubeVAO = 0;
}

// Draw a single cube part
void Robot::drawCube(Shader& shader, const glm::mat4& parent,
                     const glm::vec3& scale, const glm::vec3& translate) {
    glm::mat4 model = parent;
    model = glm::translate(model, translate);
    model = glm::scale(model, scale);
    shader.setMat4("uModel", model);
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

// Draw smooth sphere (for shoulders)
void drawSphere(Shader& shader, glm::mat4 parent, float radius) {
    const int stacks = 16, slices = 24;
    std::vector<float> vertices;
    for (int i = 0; i <= stacks; ++i) {
        float V = i / (float)stacks;
        float phi = V * M_PI;
        for (int j = 0; j <= slices; ++j) {
            float U = j / (float)slices;
            float theta = U * (M_PI * 2);
            float x = cos(theta) * sin(phi);
            float y = cos(phi);
            float z = sin(theta) * sin(phi);
            vertices.insert(vertices.end(), {x*radius, y*radius, z*radius, x, y, z});
        }
    }

    std::vector<unsigned int> indices;
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            unsigned int first = (i * (slices + 1)) + j;
            unsigned int second = first + slices + 1;
            indices.insert(indices.end(), {
                first, second, first + 1,
                second, second + 1, first + 1
            });
        }
    }

    GLuint VAO,VBO,EBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(float),vertices.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(unsigned int),indices.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    shader.setMat4("uModel",parent);
    glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,0);
    glDeleteBuffers(1,&VBO);
    glDeleteBuffers(1,&EBO);
    glDeleteVertexArrays(1,&VAO);
}

// Draw filled cylinder (for eyes)
void drawFilledCylinder(Shader& shader, glm::mat4 parent, float radius, float height) {
    const int segments = 36;
    std::vector<float> sideVerts;

    // Sides
    for (int i = 0; i <= segments; ++i) {
        float angle = i * 2.0f * M_PI / segments;
        float x = cos(angle) * radius;
        float y = sin(angle) * radius;
        sideVerts.insert(sideVerts.end(), {x, y, height * 0.5f, x, y, -height * 0.5f});
    }

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sideVerts.size() * sizeof(float), sideVerts.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    shader.setMat4("uModel", parent);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (segments + 1) * 2);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    // Caps
    auto drawCap = [&](float zOffset) {
        std::vector<float> verts;
        verts.push_back(0.0f); verts.push_back(0.0f); verts.push_back(zOffset);
        for (int i = 0; i <= segments; ++i) {
            float angle = i * 2.0f * M_PI / segments;
            verts.push_back(cos(angle) * radius);
            verts.push_back(sin(angle) * radius);
            verts.push_back(zOffset);
        }

        GLuint cVAO, cVBO;
        glGenVertexArrays(1, &cVAO);
        glGenBuffers(1, &cVBO);
        glBindVertexArray(cVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cVBO);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
        glDeleteBuffers(1, &cVBO);
        glDeleteVertexArrays(1, &cVAO);
    };

    drawCap(height * 0.5f);
    drawCap(-height * 0.5f);
}

// Draw pyramid (hat)
void drawPyramid(Shader& shader, glm::mat4 parent, float size, float height) {
    std::vector<float> verts = {-size,0,-size, size,0,-size, size,0,size, -size,0,size, 0,height,0};
    unsigned int idx[] = {0,1,2,0,2,3,0,1,4,1,2,4,2,3,4,3,0,4};
    GLuint VAO,VBO,EBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,verts.size()*sizeof(float),verts.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(idx),idx,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    shader.setMat4("uModel",parent);
    glDrawElements(GL_TRIANGLES,18,GL_UNSIGNED_INT,0);
    glDeleteBuffers(1,&VBO);
    glDeleteBuffers(1,&EBO);
    glDeleteVertexArrays(1,&VAO);
}

// Draw the entire robot hierarchy
void Robot::draw(Shader& shader) {
    glm::mat4 base(1.0f);
    base = glm::rotate(base, glm::radians(baseRotationDeg), glm::vec3(0,1,0));

    // Torso
    shader.setVec3("uBaseColor", glm::vec3(0.9f,0.4f,0.2f));
    glm::mat4 torso = glm::translate(base, glm::vec3(0,0.75f,0));
    drawCube(shader, torso, glm::vec3(0.6f,0.8f,0.3f), glm::vec3(0));

    // Head (rotates around Y)
    glm::mat4 head = glm::translate(torso, glm::vec3(0,0.5f,0));
    head = glm::rotate(head, glm::radians(headYawDeg), glm::vec3(0,1,0));
    drawCube(shader, head, glm::vec3(0.28f,0.28f,0.28f), glm::vec3(0));

    // Hat
    shader.setVec3("uBaseColor", glm::vec3(1.0f,0.15f,0.15f));
    glm::mat4 hat = glm::translate(head, glm::vec3(0.0f,0.14f,0.0f));
    hat = glm::scale(hat, glm::vec3(0.9f));
    drawPyramid(shader, hat, 0.13f, 0.20f);

    // Eyes
    shader.setVec3("uBaseColor", glm::vec3(0.05f,0.05f,0.05f));
    float eyeR=0.045f, eyeD=0.05f;
    glm::mat4 rEye=glm::translate(head,glm::vec3(0.07f,0.05f,0.15f));
    drawFilledCylinder(shader,rEye,eyeR,eyeD);
    glm::mat4 lEye=glm::translate(head,glm::vec3(-0.07f,0.05f,0.15f));
    drawFilledCylinder(shader,lEye,eyeR,eyeD);

    // Shoulder joints
    shader.setVec3("uBaseColor", glm::vec3(0.8f,0.3f,0.1f));
    glm::mat4 rShoulder=glm::translate(torso,glm::vec3(0.33f,0.05f,0));
    drawSphere(shader,rShoulder,0.09f);
    glm::mat4 lShoulder=glm::translate(torso,glm::vec3(-0.33f,0.05f,0));
    drawSphere(shader,lShoulder,0.09f);

    // Arms (Right arm is controllable/rotatable)
    shader.setVec3("uBaseColor", glm::vec3(0.9f,0.4f,0.2f));
    glm::mat4 rArm=glm::rotate(rShoulder,glm::radians(rightArmDeg),glm::vec3(0,0,1));
    drawCube(shader,rArm,glm::vec3(0.45f,0.14f,0.14f),glm::vec3(0.23f,0,0));
    glm::mat4 lArm=lShoulder;
    drawCube(shader,lArm,glm::vec3(0.45f,0.14f,0.14f),glm::vec3(-0.23f,0,0));

    // Legs (animated)

    // Right leg: position, then rotate
    shader.setVec3("uBaseColor", glm::vec3(0.7f,0.35f,0.15f));
    glm::mat4 rLegParent = glm::translate(torso, glm::vec3(0.16f,-0.55f,0.0f));
    rLegParent = glm::rotate(rLegParent, glm::radians(rightLegDeg), glm::vec3(1,0,0));
    drawCube(shader, rLegParent, glm::vec3(0.22f,0.50f,0.22f), glm::vec3(0));

    // Left leg
    glm::mat4 lLegParent = glm::translate(torso, glm::vec3(-0.16f,-0.55f,0.0f));
    lLegParent = glm::rotate(lLegParent, glm::radians(leftLegDeg), glm::vec3(1,0,0));
    drawCube(shader, lLegParent, glm::vec3(0.22f,0.50f,0.22f), glm::vec3(0));
}