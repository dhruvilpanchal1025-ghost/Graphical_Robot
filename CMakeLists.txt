cmake_minimum_required(VERSION 3.10)
project(robot_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OpenGL_GL_PREFERENCE GLVND)
# ------------------------------------------------
# Find OpenGL (system-provided)
# ------------------------------------------------
find_package(OpenGL REQUIRED)

# ------------------------------------------------
# Source files
# ------------------------------------------------
add_executable(robot_demo
    src/main.cpp
    src/robot.cpp
    src/scene.cpp
    src/camera.cpp
    src/Shader.cpp
)

# ------------------------------------------------
# GLAD library
# ------------------------------------------------
add_library(glad external/glad/src/glad.c)

# Force GLAD to include its own headers
target_include_directories(glad PUBLIC
    ${CMAKE_SOURCE_DIR}/external/glad/include
)

# ------------------------------------------------
# GLFW library (local)
# ------------------------------------------------
add_subdirectory(external/glfw)

# ------------------------------------------------
# Include directories
# ------------------------------------------------
target_include_directories(robot_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/glad/include
    ${CMAKE_SOURCE_DIR}/external/glm
    ${CMAKE_SOURCE_DIR}/external/glfw/include
)

# ------------------------------------------------
# Link everything together
# ------------------------------------------------
target_link_libraries(robot_demo PRIVATE
    glad
    glfw
    ${OPENGL_LIBRARIES}
    GL
    GLU
)

# ------------------------------------------------
# Compiler options (optional for clarity)
# ------------------------------------------------
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(robot_demo PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()