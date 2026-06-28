#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shaderClass.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include "Movement.h"
#include <cmath>

static float radians(float degrees) {
    return degrees * 3.14159265358979323846f / 180.0f;
}

void perspective(float fovRadians, float aspect, float nearPlane, float farPlane, float projection[16]) {
    float f = 1.0f / std::tan(fovRadians / 2.0f);
    projection[0] = f / aspect;
    projection[1] = 0.0f;
    projection[2] = 0.0f;
    projection[3] = 0.0f;
    projection[4] = 0.0f;
    projection[5] = f;
    projection[6] = 0.0f;
    projection[7] = 0.0f;
    projection[8] = 0.0f;
    projection[9] = 0.0f;
    projection[10] = (farPlane + nearPlane) / (nearPlane - farPlane);
    projection[11] = -1.0f;
    projection[12] = 0.0f;
    projection[13] = 0.0f;
    projection[14] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
    projection[15] = 0.0f;
}

void multiplyMat4(const float a[16], const float b[16], float out[16]) {
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k) {
                sum += a[k * 4 + row] * b[col * 4 + k];
            }
            out[col * 4 + row] = sum;
        }
    }
}

// Callback function to handle window resize events
// Updates the OpenGL viewport to match the new window dimensions
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
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
        1, 2, 0   // Second triangle
    };

    // Initialize shader program from external shader files
    Shader shader("../Resources/Shaders/default.vert", "../Resources/Shaders/default.frag");
    
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

        float timeValue =glfwGetTime();
        float greenValue=sin(timeValue)/ 2.0f + 0.5f;
        int vertexColorLocation = glGetUniformLocation(shader.ID, "ourColor");
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);



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
