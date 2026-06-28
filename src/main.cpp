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

    MovementController movement(window);

    // Define vertex positions for a square (two triangles)
    float vertices[] = {
    // positions
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    };
 
    unsigned int indices[] = {
        0,1,2, 2,3,0,   // back face
        4,5,6, 6,7,4,   // front face
        0,4,7, 7,3,0,   // left face
        1,5,6, 6,2,1,   // right face
        3,2,6, 6,7,3,   // top face
        0,1,5, 5,4,0    // bottom face
    };


    // Initialize shader program from external shader files
    Shader shader("../Resources/Shaders/default.vert", "../Resources/Shaders/default.frag");
    
    // Create and configure vertex array object.
    VAO vao;
    
    // Create and upload vertex buffer object with vertex data.
    VBO vbo(vertices, sizeof(vertices));
    
    // Create and upload element buffer object with index data.
    EBO ebo(indices, sizeof(indices));

    // Link the VBO and EBO into the VAO so the vertex attributes are bound once.
    vao.Bind();
    ebo.Bind();
    vao.LinkVBO(vbo, 0);  // Bind VBO to layout location 0.
    vao.Unbind();
    ebo.Unbind();
    
    glEnable(GL_DEPTH_TEST);

    // Main render loop - runs until the user closes the window.
    while (!glfwWindowShouldClose(window)) {
        // Update the movement controller first so the view matrix reflects the latest camera pose.
        movement.Update();

        // Clear the framebuffer with a grey background and depth buffer.
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate shader program and render the geometry.
        shader.Activate();

        // Animate the cube's color over time.
        float timeValue = static_cast<float>(glfwGetTime());
        float greenValue = std::sin(timeValue) / 2.0f + 0.5f;
        int vertexColorLocation = glGetUniformLocation(shader.ID, "ourColor");
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        // Compute the MVP matrix using the projection and camera view matrices.
        int mvpLocation = glGetUniformLocation(shader.ID, "MVP");
        float aspect = 800.0f / 600.0f;
        float projection[16];
        perspective(radians(45.0f), aspect, 0.1f, 100.0f, projection);

        float mvp[16];
        multiplyMat4(projection, movement.GetViewMatrix(), mvp);
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, mvp);

        // Draw the cube using the VAO bound to the shader.
        vao.Bind();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Swap buffers and poll for input events.
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
