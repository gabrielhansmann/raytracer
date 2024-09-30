#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <GL/glew.h>
#include <glm/glm.hpp>

// Structure to store vertex array object and vertex buffer object
struct VertexObjects {
    GLuint VAO;
    GLuint VBO;
};

// Structures for Plane geometry
struct Plane {
    glm::vec3 point;
    float pad1 = 0.0f;
    glm::vec3 normal;
    float pad2 = 0.0f;
    glm::vec3 color;
    float reflectivity;

    Plane(const glm::vec3& p, const glm::vec3& n, const glm::vec3& c, const float r) : point(p), normal(n), color(c), reflectivity(r) {}
};

//structures for Sphere geometry
struct Sphere {
    glm::vec3 center;
    float radius;
    glm::vec3 color;
    float reflectivity;

    Sphere(const glm::vec3& ce, const float ra, const glm::vec3 co, const float re) : center(ce), radius(ra), color(co), reflectivity(re) {}
};

//Structure for Light
struct Light {
    glm::vec3 position;
    float pad1 = 0.0f;
    glm::vec3 color;
    float pad2 = 0.0f;

    Light(const glm::vec3& p, const glm::vec3& c) : position(p), color(c) {}
};

//Structure for FreeType character
struct Character {
    GLuint textureID;   // ID handle of the glyph texture
    glm::ivec2 size;    // Size of glyph
    glm::ivec2 bearing; // Offset from baseline to left/top of glyph
    GLuint advance;     // Offset to advance to next glyph
};

#endif // GEOMETRY_H