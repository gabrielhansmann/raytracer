#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

// Function to load shader code from a file
std::string loadShaderSource(const std::string& filepath) {
    std::ifstream shaderFile(filepath);  
    if (!shaderFile.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }
    
    std::stringstream shaderStream;

    shaderStream << shaderFile.rdbuf();
    shaderFile.close();

    return shaderStream.str();
}

// Function to compile a shader
GLuint compileShader(GLuint type, const std::string& source) {    
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check for compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

// Function to create a shader program
GLuint createShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);


    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);    
    
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Function to delete a shader program
void deleteShaderProgram(GLuint shaderProgram) {
    glDeleteProgram(shaderProgram);
}

// Function to load and compile the compute shader
GLuint loadComputeShader(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open compute shader file: " << path << std::endl;
        return 0;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    const char* src = source.c_str();

    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPUTE::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(shader); // We can delete the shader after linking

    return program;
}

// Function to create and bind SSBO for geometry data
GLuint createSSBO(const std::vector<Sphere>& spheres, const std::vector<Plane>& planes, const std::vector<Light>& lights) {
    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

    size_t sphereSize = spheres.size() * sizeof(Sphere);
    size_t planeSize = planes.size() * sizeof(Plane);
    size_t lightSize = lights.size() * sizeof(Light);

    glBufferData(GL_SHADER_STORAGE_BUFFER, sphereSize + planeSize + lightSize, nullptr, GL_DYNAMIC_DRAW);

    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sphereSize, spheres.data());
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, sphereSize, planeSize, planes.data());
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, sphereSize + planeSize, lightSize, lights.data());

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    return ssbo;
}

// Function to create a 2D texture to store the output of the compute shader
GLuint createTexture(int width, int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

// Function to set the uniforms for the compute shader
void setComputeShaderUniforms(GLuint computeProgram, int width, int height, const glm::vec3& cameraPos, const glm::vec3& cameraDir, float focalLength) {
    glUseProgram(computeProgram);

    // Set uniform values for screen width, height, camera position, and direction
    glUniform1i(glGetUniformLocation(computeProgram, "screenWidth"), width);
    glUniform1i(glGetUniformLocation(computeProgram, "screenHeight"), height);
    glUniform3f(glGetUniformLocation(computeProgram, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
    glUniform3f(glGetUniformLocation(computeProgram, "cameraDir"), cameraDir.x, cameraDir.y, cameraDir.z);
    glUniform1f(glGetUniformLocation(computeProgram, "focalLength"), focalLength);
}

// Function to dispatch the compute shader for ray tracing
void dispatchComputeShader(GLuint computeProgram, GLuint texture, int width, int height) {

    // Use the compute shader program
    glUseProgram(computeProgram);

    // Bind the texture for writing
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    // Define workgroup size
    int workgroupSizeX = 16;
    int workgroupSizeY = 16;

    // Calculate the number of workgroups needed
    int numGroupsX = (width + workgroupSizeX - 1) / workgroupSizeX;
    int numGroupsY = (height + workgroupSizeY - 1) / workgroupSizeY;

    // Dispatch the compute shader
    glDispatchCompute(numGroupsX, numGroupsY, 1);

    // Ensure all writes to the texture have finished
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

// Function to render a full-screen quad with a texture
void renderQuadWithTexture(GLuint texture, GLuint shaderProgram, VertexObjects quadVO) {
    // Use the shader program for rendering the quad
    glUseProgram(shaderProgram);

    // Bind the texture to the quad
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set the sampler uniform to use texture unit 0
    glUniform1i(glGetUniformLocation(shaderProgram, "screenTexture"), 0);

    // Draw the quad
    glBindVertexArray(quadVO.VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}