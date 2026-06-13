#include <iostream>
#include <glad/glad.h>//include the glad.h
#include <GLFW/glfw3.h>//include the glfw3.h
#include <cmath>

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
" gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";


const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor; \n"
"void main()\n"
"{\n"
"FragColor = vec4(0.5f, 0.1f, 0.02f, 1.0f);\n"
"}\n\0";





//In this code is for intializing the window using glad and glfw 
int main(){

    glfwInit();//initialize the glfw
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLfloat vertices[]={
    -0.5f,-0.5f, 0.0f,
     0.5f,-0.5f,0.0f,
     0.0f, 0.0f,0.0,
     0.0f, 0.5f,0.0f
    };//the vertices of the triangle




    GLFWwindow* window = glfwCreateWindow(800,800,"LearnOpenGL",NULL,NULL);//create a window
    //GLFWwindow is a data type representing a window in a glfw ,it is a pointer to structure that contains the info of the window
    if(window ==NULL){
        std::cout<<"Failed to create GLFW window"<<std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);//make the window the current context

    gladLoadGL();


    glViewport(0,0,800,800);//set the viewport of openGL in the window
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);//create a vertex shader object
    glShaderSource(vertexShader,1,&vertexShaderSource,NULL);//attach the vertex shader source code to the vertex shader object
    glCompileShader(vertexShader);//compile the vertex shader

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);//create a fragment shader object
    glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);//attach the fragment shader source code to the fragment shader object
    glCompileShader(fragmentShader);//compile the fragment shader

    GLuint shaderProgram= glCreateProgram();//create a shader program object
    glAttachShader(shaderProgram,vertexShader);//attach the vertex shader to the shader program
    glAttachShader(shaderProgram,fragmentShader);//attach the fragment shader to the shader program
    glLinkProgram(shaderProgram);//link the shader program

    glDeleteShader(vertexShader);//delete the vertex shader object
    glDeleteShader(fragmentShader);//delete the fragment shader object


    GLuint VAO,VBO;//create a vertex buffer object and a vertex array object
    glGenVertexArrays(1,&VAO);//generate a vertex array object
    glGenBuffers(1,&VBO);//generate a vertex buffer object
    glBindVertexArray(VAO);//bind the vertex array object
    glBindBuffer(GL_ARRAY_BUFFER,VBO);//bind the vertex buffer object to the GL_ARRAY_BUFFER target
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);//copy the vertex data to the vertex buffer object
    
    
    
    glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);//set the vertex attribute pointer
    glEnableVertexAttribArray(0);//enable the vertex attribute array
    
    glBindBuffer(GL_ARRAY_BUFFER,0);//unbind the vertex buffer object
    glBindVertexArray(0);//unbind the vertex array object
    
    
    glClearColor(0.5f,0.3f,0.3f,1.0f);//set the clear color the last parameter is the transparency
    glClear(GL_COLOR_BUFFER_BIT);//clear the color buffer
    glfwSwapBuffers(window);//swap the color buffer to display the clear color


    

    while(!glfwWindowShouldClose(window)){

        glClearColor(0.5f,0.3f,0.3f,1.0f);//set the clear color the last parameter is the transparency
        glClear(GL_COLOR_BUFFER_BIT);//clear the color buffer
        
        glUseProgram(shaderProgram);//use the shader program
        glBindVertexArray(VAO);//bind the vertex array object
        
        glDrawArrays(GL_TRIANGLE_STRIP,0,4);//draw the triangle
        glfwSwapBuffers(window);//swap the color buffer to display the triangle


        glfwPollEvents();//check if there are any events and call the corresponding functions
        
    }



    glDeleteVertexArrays(1,&VAO);//delete the vertex array object
    glDeleteBuffers(1,&VBO);//delete the vertex buffer object
    glDeleteProgram(shaderProgram);//delete the shader program object


    glfwDestroyWindow(window);//destroy the window
    glfwTerminate();//terminate the glfw

    return 0;
}