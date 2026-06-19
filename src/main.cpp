#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shaderClass.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"

// Callback function to handle window resize events
// Updates the OpenGL viewport to match the new window dimensions
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Handle user input for closing the application
// Exits the render loop when ESC key is pressed
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    // Initialize GLFW - window and context management library
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure OpenGL context version (3.3 core profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the application window
    GLFWwindow* window = glfwCreateWindow(800, 600, "PhysicsEngine", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Set the OpenGL context for this thread
    glfwMakeContextCurrent(window);
    
    // Load OpenGL function pointers using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Setup viewport and window resize callback
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Define vertex positions for a square (two triangles)
    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };

    // Define element indices for indexed drawing
    unsigned int indices[] = {
        0, 1, 3,  // First triangle
        1, 2, 3   // Second triangle
    };

    // Initialize shader program from external shader files
    Shader shader("Resources/Shaders/default.vert", "Resources/Shaders/default.frag");
    
    // Create and configure vertex array object
    VAO vao;
    
    // Create and upload vertex buffer object with vertex data
    VBO vbo(vertices, sizeof(vertices));
    
    // Create and upload element buffer object with indices
    EBO ebo(indices, sizeof(indices));

    // Link vertex attributes to the VAO
    vao.Bind();
    ebo.Bind();
    vao.LinkVBO(vbo, 0);  // Bind VBO to layout location 0
    vao.Unbind();
    ebo.Unbind();

    // Main render loop - runs until window is closed
    while (!glfwWindowShouldClose(window)) {
        // Process user input
        processInput(window);

        // Clear the framebuffer with a solid color
        glClearColor(0.0f, 0.4f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Activate shader program and render the geometry
        shader.Activate();
        vao.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Swap framebuffers and poll for events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up GPU resources
    vao.Delete();
    vbo.Delete();
    ebo.Delete();
    shader.Delete();
    
    // Cleanup and exit
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
