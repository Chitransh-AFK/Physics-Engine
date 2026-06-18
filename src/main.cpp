#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Simple vertex shader source: transforms vertex positions to clip space.
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

// Simple fragment shader source: outputs a static yellow color.
const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
        "FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
    "}\0";

//resize the drawing w.r.t to window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
//exits the code when pressed esc key
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    //checking if glfw is initialized correctly
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    //giving glfw the version of glfw and gl we want to import 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //creating a window object
    GLFWwindow* window = glfwCreateWindow(800, 600, "PhysicsEngine", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    //seting the context of the window to current
    glfwMakeContextCurrent(window);
    //checking the initialization of glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Vertex positions for a square composed of two triangles.
    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };

    // Element indices define the two triangles that make up the square.
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    //creating shader,compiling it and linking it to shaderProgram

    unsigned int vertexShader,fragmentShader,shaderProgram;

    vertexShader=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
    glCompileShader(vertexShader);
    
    fragmentShader=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
    glCompileShader(fragmentShader);

    shaderProgram=glCreateProgram();
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    //linking shader Program to the GPU
    glLinkProgram(shaderProgram);
    
    //checking for the error logs in the shaders program
    int successV,successF,successP;
    char infoV[512],infoF[512],infoP[512];
    glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&successV);
    glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,&successF);
    glGetProgramiv(shaderProgram,GL_LINK_STATUS,&successP);
    

    if(!successV || !successF||!successP){
        glGetShaderInfoLog(vertexShader,512,NULL,infoV);
        glGetShaderInfoLog(fragmentShader,512,NULL,infoF);
        glGetProgramInfoLog(shaderProgram,512,NULL,infoP);

        std::cout<<"ERROR IN COMPILING VERTEX SHADER: "<<infoV<<std::endl;
        std::cout<<"ERROR IN COMPILING FRAGMENT SHADER: "<<infoF<<std::endl;
        std::cout<<"ERROR in LINKING SHADERS: "<<infoP<<std::endl;
    }

    

    //creating Vertex buffer object,Vertex array object and Element buffer object 
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind the VAO first, then configure vertex buffer and attribute pointers.
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Upload the element index data for drawing via glDrawElements.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    
    
    
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Main render loop: draw the square composed of two triangles until the window is closed.
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glUseProgram(shaderProgram);
        
        glClearColor(0.0f, 0.4f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        // Bind the VAO and draw using indexed geometry.
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up GPU resources.
    
    // The shader objects are no longer needed after program linking.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    //Delete Buffers
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1,&EBO);
    //destroy the window and terminate the glfw
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
