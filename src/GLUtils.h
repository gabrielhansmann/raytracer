#ifndef GLUTILS_H
#define GLUTILS_H

#include "Geometry.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// #include <ft2build.h>
// #include FT_FREETYPE_H
// #include <map>
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>

// Function to initialize GLFW, create a window, and initialize GLEW
GLFWwindow* initializeOpenGL(int width, int height, const char* title);

// Function to create a Vertex Array Object and a Vertex Buffer Object for the screen-filling quad
VertexObjects createVertexObjectsForQuad(const GLfloat* vertices, size_t size);

// Function to delete a Vertex Array Object and a Vertex Buffer Object
void deleteVertexObjects(VertexObjects vo);

// Function to calculate the frames per second
double calculateFPS();

#endif // GLUTILS_H
