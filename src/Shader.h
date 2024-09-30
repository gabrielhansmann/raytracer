#ifndef SHADER_H
#define SHADER_H

#include "Geometry.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

// Function to load a shader source from a file
std::string loadShaderSource(const std::string& filepath);

// Function to compile a shader
GLuint compileShader(GLenum type, const std::string& source);

// Function to create a shader program from vertex and fragment shaders
GLuint createShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);

// Function to delete a shader program
void deleteShaderProgram(GLuint shaderProgram);

// Function to load and compile the compute shader
GLuint loadComputeShader(const std::string& path);

// Function to create and bind SSBO for geometry data
GLuint createSSBO(const std::vector<Sphere>& spheres, const std::vector<Plane>& planes, const std::vector<Light>& lights);

// Function to create a 2D texture to store the output of the compute shader
GLuint createTexture(int width, int height);

// Function to dispatch the compute shader for ray tracing
void dispatchComputeShader(GLuint computeProgram, GLuint texture, int width, int height);

// Function to set uniforms for the compute shader
void setComputeShaderUniforms(GLuint computeProgram, int width, int height, const glm::vec3& cameraPos, const glm::vec3& cameraDir, float focalLength);

// Function to render a full-screen quad with a texture
void renderQuadWithTexture(GLuint texture, GLuint shaderProgram, VertexObjects vo);

#endif // SHADER_H