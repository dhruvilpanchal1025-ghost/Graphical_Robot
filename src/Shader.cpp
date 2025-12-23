#include "Shader.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

// Constructor: load and compile shaders, then link the program
Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    std::string vCode = readFile(vertexPath);
    std::string fCode = readFile(fragmentPath);
    const char* vSrc = vCode.c_str();
    const char* fSrc = fCode.c_str();

    // Compile vertex shader
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vSrc, nullptr);
    glCompileShader(vs);
    checkCompileErrors(vs, "VERTEX");

    // Compile fragment shader
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fSrc, nullptr);
    glCompileShader(fs);
    checkCompileErrors(fs, "FRAGMENT");

    // Link shaders into program
    ID = glCreateProgram();
    glAttachShader(ID, vs);
    glAttachShader(ID, fs);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // Delete shaders after linking
    glDeleteShader(vs);
    glDeleteShader(fs);
}

// Activate the shader program
void Shader::use() const { glUseProgram(ID); }

// Set Mat4 uniform
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
}

// Set Vec3 uniform
void Shader::setVec3(const std::string& name, const glm::vec3& v) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniform3fv(loc, 1, &v[0]);
}

// Set Float uniform
void Shader::setFloat(const std::string& name, float v) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniform1f(loc, v);
}

// Set Int uniform
void Shader::setInt(const std::string& name, int v) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniform1i(loc, v);
}

// Read shader source from file path
std::string Shader::readFile(const char* path) {
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    if (!file) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << path << "\n";
    }
    return ss.str();
}

// Check for shader compile or link errors
void Shader::checkCompileErrors(unsigned int obj, const std::string& type) {
    int success; char log[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(obj, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(obj, 1024, nullptr, log);
            std::cerr << type << " compile error:\n" << log << "\n";
        }
    } else {
        glGetProgramiv(obj, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(obj, 1024, nullptr, log);
            std::cerr << "link error:\n" << log << "\n";
        }
    }
}