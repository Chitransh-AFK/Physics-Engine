#include  <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
void framebuffer_size_callback(GLFWwindow* window,int wdith,int height){
    glViewport(0,0,wdith,height);
}
void processInput(GLFWwindow* window){
    if(glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS){
        glfwSetWindowShouldClose(window,true);
        
    }
}
int main(){

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    float vertex[]={
        0.5f,0.0f,0.0f,
        -0.5f,-0.5f,0.0f,
        0.5f,-0.5f,0.0f
    };

    unsigned int VBO=0;
    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertex),vertex,GL_STATIC_DRAW);
    




    GLFWwindow* window = glfwCreateWindow(800,600,"PhysicsEngine",NULL,NULL);
    if (window==NULL){
        std::cout<<"Window Unable to start"<<std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout<<"Failed to intialize GLAD"<<std::endl;
        return -1;
    }

    glViewport(0,0,800,600);
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
    
    while(!glfwWindowShouldClose(window)){
        
        //inputs
        processInput(window);

        //render
        glClearColor(0.2f,0.4f,0.2f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        //check all commands and events
        glfwSwapBuffers(window);
        glfwPollEvents();



    }


    glfwTerminate();
    return 0;

}