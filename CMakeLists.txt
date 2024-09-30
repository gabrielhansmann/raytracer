cmake_minimum_required(VERSION 3.10)

# Set the project name and C++ standard
project(raytracer VERSION 1.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(raytracer src/main.cpp src/Shader.cpp src/GLUtils.cpp)

# Find and include GLM
find_package(glm REQUIRED)
target_include_directories(raytracer PRIVATE ${GLM_INCLUDE_DIRS})

# Find and link OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(raytracer PRIVATE ${OPENGL_LIBRARIES})

# Find and link GLEW
find_package(GLEW REQUIRED)
target_link_libraries(raytracer PRIVATE GLEW::GLEW)

# Find and link GLFW
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(raytracer PRIVATE glfw)

# Include directories (if needed)
target_include_directories(raytracer PRIVATE
    ${GLEW_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
)

# Add definitions
set(SHADER_DIR ${CMAKE_SOURCE_DIR}/src/shaders)
add_definitions(-DSHADER_DIR="${SHADER_DIR}")


