#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int ID;

    // Load, compile, and link shaders
    Shader(const char* vertexPath, const char* fragmentPath);
    void use() const;

    // Set uniform values
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec3(const std::string &name, const glm::vec3 &vec) const;
    void setFloat(const std::string &name, float value) const;
    void setInt(const std::string &name, int value) const;

private:
    std::string readFile(const char* path);
    void checkCompileErrors(unsigned int shader, const std::string& type);
};


#endif