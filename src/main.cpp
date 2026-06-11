#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window=glfwCreateWindow(800,900,"OpenGL",NULL,NULL);

    if(window==NULL){
        std::cout<<"Failed to create GLFW window"<<std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    gladLoadGL();



    glViewport(0,0,800,900);
    glClearColor(0.2f,0.3f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
    }
    glfwTerminate();
    return  0;
}
