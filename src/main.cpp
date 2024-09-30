#include "GLUtils.h"
#include "Shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

int screenWidth = 800;
int screenHeight = 600;
// Texture to store the result of the ray tracing
GLuint texture;
// Set up camera
glm::vec3 cameraPos(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDir(0.0f, 0.0f, -1.0f);
float focalLength = 1.0f;
// Yaw and pitch angles
float yaw = -90.0f;  // Yaw is initialized to -90.0 degrees since we're looking at -Z by default
float pitch = 0.0f;  // Pitch starts at 0 degrees
// Sensitivity and speed
float mouseSensitivity = 0.1f;
// Keep track of the last mouse position
float lastX = screenWidth/2.0f, lastY = screenHeight/2.0f;
bool firstMouse = true;

GLuint defineAndBindGeometry(glm::vec3 cameraPos, glm::vec3 cameraDir) {
    float time = glfwGetTime();
    
    // Define geometry
    std::vector<Sphere> spheres = {
        {{0.3f, 0.4f * sin(time), -0.5f}, 0.2f, {1.0f, 1.0f, 1.0f}, 0.0f},
        {{-0.3f, -0.15, 0.35f}, 0.15f, {0.0f, 1.0f, 0.0f}, 0.0f},
        {{0.2*sin(3*time), -0.55, -0.2f}, 0.3f, {0.0f, 0.0f, 1.0f}, 0.0f}
    };
    
    // Box around Camera
    float boxSize = 1.0f;
    std::vector<Plane> planes = {
        {{0.0f, 0.0f, -boxSize}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, 0.0f},  // Front plane
        {{0.0f, 0.0f, boxSize}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, 0.0f},  // Back plane
        {{-boxSize, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, 0.0f},  // Left Plane
        {{boxSize, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.5f, 0.5f, 1.0f}, 0.0f},  // Right Plane
        {{0.0f, -boxSize, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.5f, 0.5f}, 0.0f},  // Bottom Plane
        {{0.0f, boxSize, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.0f, 0.5f}, 0.0f}   // Top Plane
    };

    std::vector<Light> lights = {
        {{-0.4f, 0.6f, -0.3f}, {1.0f, 1.0f, 1.0f}}
    };

    // Create and bind SSBO for geometry data
    GLuint ssbo = createSSBO(spheres, planes, lights);
    return ssbo;
}

// Window Resizing with GLFW and generating suitable texture
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    screenWidth = width;
    screenHeight = height;
    glDeleteTextures(1, &texture);
    texture = createTexture(screenWidth, screenHeight);
}

void toggleFullScreenWithF11(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
        if(!glfwGetWindowAttrib(window, GLFW_MAXIMIZED)) {
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
            screenWidth = mode->width;
            screenHeight = mode->height;
            glDeleteTextures(1, &texture);
            texture = createTexture(screenWidth, screenHeight);
        } else {
            //TODO: Restore windowed mode
        }
    }
}

// Cursor Position Callback
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;  // Reversed: y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;
    // Apply sensitivity to the offset
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;
    // Update yaw and pitch
    yaw += xoffset;
    pitch += yoffset;
    // Constrain the pitch angle to avoid flipping
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    // Calculate the new camera direction
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraDir = glm::normalize(front);

    // Clamp cursor position
    glfwSetCursorPos(window, screenWidth/2.0, screenHeight/2.0);
    lastX = screenWidth/2.0;
    lastY = screenHeight/2.0;
}

int main() {
    // Initialize OpenGL, create window, and initialize GLEW
    GLFWwindow* window = initializeOpenGL(screenWidth, screenHeight, "Ray Tracing");
    if(!window) return -1;

    // Define and Bind geometry
    GLuint ssbo = defineAndBindGeometry(cameraPos, cameraDir);

    // Load, compile and link quad shader
    std::string quadVertexShaderSource = loadShaderSource(std::string(SHADER_DIR) + "/quad.vert");
    std::string quadFragmentShaderSource = loadShaderSource(std::string(SHADER_DIR) + "/quad.frag");
    GLuint quadShaderProgram = createShaderProgram(quadVertexShaderSource, quadFragmentShaderSource);
    // Screen-Filling quad vertices and texture coordinates
    float quadVertices[] = {
        // Positions    // Texture Coords
        -1.0f,  1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f
    };
    // Create and set up the Vertex Objects using the utility function
    VertexObjects quadVO = createVertexObjectsForQuad(quadVertices, sizeof(quadVertices));

    // Load compute shader
    GLuint computeProgram = loadComputeShader(std::string(SHADER_DIR) + "/raytracing.comp");
    // Create texture to store the result
    texture = createTexture(screenWidth, screenHeight);

    // Set up the framebuffer size callback
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    // Update camera direction
    glfwSetCursorPosCallback(window, cursorPosCallback);
    // Hide the cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    double realFPS = 0.0;
    double oldFPS = 0.0;
    //Rendering loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Close window on ESC press
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
        // Fullscreen on F11 press
        toggleFullScreenWithF11(window);

        // Calculate FPS
        double calculatedFPS = calculateFPS();
        if(calculatedFPS != -1.0) realFPS = calculatedFPS;
        if(oldFPS != realFPS) std::cout << "FPS: " << realFPS << std::endl;
        oldFPS = realFPS;
        
        // Update camera position
        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += 0.01f * cameraDir ;
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= 0.01f * cameraDir;
        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= 0.01f * glm::normalize(glm::cross(cameraDir, glm::vec3(0.0f, 1.0f, 0.0f)));
        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += 0.01f * glm::normalize(glm::cross(cameraDir, glm::vec3(0.0f, 1.0f, 0.0f)));
        if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) cameraPos += 0.01f * glm::vec3(0.0f, 1.0f, 0.0f);
        if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) cameraPos -= 0.01f * glm::vec3(0.0f, 1.0f, 0.0f);
        // Clamp the camera position to stay within the range (-1, 1) in all directions
        cameraPos.x = glm::clamp(cameraPos.x, -0.999f, 0.999f);
        cameraPos.y = glm::clamp(cameraPos.y, -0.999f, 0.999f);
        cameraPos.z = glm::clamp(cameraPos.z, -0.999f, 0.999f);

        // Update SSBO
        glDeleteBuffers(1, &ssbo);
        ssbo = defineAndBindGeometry(cameraPos, cameraDir);
        // Update Focal Length
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) focalLength += 0.01f;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) focalLength -= 0.01f;

        // Set compute shader uniforms
        setComputeShaderUniforms(computeProgram, screenWidth, screenHeight, cameraPos, cameraDir, focalLength);
        // Dispatch the compute shader
        dispatchComputeShader(computeProgram, texture, screenWidth, screenHeight);
        // Render the full-screen quad with the texture (implement renderQuadWithTexture yourself)
        renderQuadWithTexture(texture, quadShaderProgram, quadVO);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Cleanup
    glDeleteBuffers(1, &ssbo);
    glDeleteProgram(computeProgram);
    glDeleteTextures(1, &texture);
    deleteVertexObjects(quadVO);
    deleteShaderProgram(quadShaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
