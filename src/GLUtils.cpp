#include "GLUtils.h"

GLFWwindow* initializeOpenGL(int width, int height, const char* title) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    // Set GLFW options (optional, depending on your needs)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Use OpenGL 3.x
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use core profile
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Forward compatible for MacOS

    // Create GLFW window
    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    // Make the context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    // Set the viewport size (optional, can be handled elsewhere)
    glViewport(0, 0, width, height);

    return window;
}

VertexObjects createVertexObjectsForQuad(const GLfloat* vertices, size_t size) {
    VertexObjects vo;

    // Generate and bind the Vertex Array Object
    glGenVertexArrays(1, &vo.VAO);
    glBindVertexArray(vo.VAO);

    // Generate and bind the Vertex Buffer Object
    glGenBuffers(1, &vo.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vo.VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind the VAO (optional)
    glBindVertexArray(0);

    return vo;  // Return the VAO and VBO
}

void deleteVertexObjects(VertexObjects vo) {
    glDeleteVertexArrays(1, &vo.VAO);
    glDeleteBuffers(1, &vo.VBO);
}

double calculateFPS() {
    static double previousTime = 0.0;
    static int frameCount = 0;
    double currentTime = glfwGetTime();  // Get current time in seconds
    frameCount++;

    // Calculate FPS every second
    if (currentTime - previousTime >= 1.0) {
        double fps = double(frameCount) / (currentTime - previousTime);

        // Reset for the next calculation
        previousTime = currentTime;
        frameCount = 0;

        return fps;  // Return the calculated FPS
    }

    return -1.0;  // Return -1 if it's not time to update FPS yet
}