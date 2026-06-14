#include <iostream>
#include <glad/glad.h>//include the glad.h
#include <GLFW/glfw3.h>//include the glfw3.h
#include <cmath>
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

//In this code is for intializing the window using glad and glfw 
int main(){

    glfwInit();//initialize the glfw
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLfloat vertices[]={
   -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, //lower left corner
    0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, //Lower right corner
    0.0f, 0.5f * float (sqrt(3)) * 2/3, 0.0f,//upper corner
    -0.5f/2, 0.5f* float(sqrt(3)) /6, 0.0f,//Inner left corner
    0.5f/2, 0.5f* float(sqrt(3)) / 6, 0.0f,//inner right corner
    0.0f, -0.5f * float(sqrt(3)) / 3, 0.0f // Inner down corner
    };//the vertices of the triangle

    GLuint indices[]={
        0,3,5,//lower left triangle
        3,2,4,//upper triangle
        5,4,1//lower right triangle
    };


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
    
    Shader* shaderProgram = nullptr;
    try {
        shaderProgram = new Shader("../Resources/Shaders/default.vert","../Resources/Shaders/default.frag");//create a shader program object and build the shader program
    } catch (const std::exception& e) {
        std::cerr << "Shader load error: " << e.what() << std::endl;
        glfwTerminate();
        return -1;
    }

    VAO VAO1;//create a vertex array object
    VAO1.Bind();//bind the vertex array object

    VBO VBO1(vertices,sizeof(vertices));//create a vertex buffer object and link it to the vertices array
    EBO EBO1(indices,sizeof(indices));//create an element buffer object and link it to the indices array
    
    VAO1.LinkVBO(VBO1,0);//link the vertex buffer object to the vertex array object
    VAO1.Unbind();//unbind the vertex array object
    VBO1.Unbind();//unbind the vertex buffer object
    EBO1.Unbind();//unbind the element buffer object


   

    while(!glfwWindowShouldClose(window)){

        glClearColor(0.5f,0.3f,0.3f,1.0f);//set the clear color the last parameter is the transparency
        glClear(GL_COLOR_BUFFER_BIT);//clear the color buffer
        
        shaderProgram->Activate();//activate the shader program
        VAO1.Bind();//bind the vertex array object

        
        glDrawElements(GL_TRIANGLES,9, GL_UNSIGNED_INT,0);//draw the triangle
        glfwSwapBuffers(window);//swap the color buffer to display the triangle


        glfwPollEvents();//check if there are any events and call the corresponding functions
        
    }



    //delete the element buffer object
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    shaderProgram->Delete();
    delete shaderProgram;

    glfwDestroyWindow(window);//destroy the window
    glfwTerminate();//terminate the glfw

    return 0;
}