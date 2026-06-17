# Lesson 2: Creating a Window

> Original study notes for the *Physics_Engine* learning journal.
> Reference: https://learnopengl.com/Getting-started/Creating-a-window (not reproduced — concepts explained independently)

## Step 1: Initialize GLFW and set window hints

Before creating a window, GLFW needs to know which OpenGL version and profile to request:

```cpp
glfwInit();
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // required on macOS
#endif
```

Hints configure the *next* window GLFW creates — they don't take effect retroactively.

## Step 2: Create the window object

```cpp
GLFWwindow* window = glfwCreateWindow(800, 600, "Physics Engine", nullptr, nullptr);
if (!window) {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return -1;
}
glfwMakeContextCurrent(window);
```

`glfwMakeContextCurrent` is important: OpenGL calls operate on whichever context is "current" on the calling thread. Forgetting this line means every subsequent OpenGL call silently does nothing (or crashes).

## Step 3: Load function pointers with GLAD

Modern OpenGL function addresses aren't resolved at link time — they have to be queried from the driver at runtime, which is exactly what GLAD's generated loader does:

```cpp
if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD\n";
    return -1;
}
```

This **must** happen after `glfwMakeContextCurrent` and before any other OpenGL call — there's no context to query function pointers from until this point.

## Step 4: Define the viewport

```cpp
glViewport(0, 0, 800, 600);
```

This tells OpenGL how to map **normalized device coordinates** (the -1 to 1 cube that all rendered geometry ends up in) to actual pixel coordinates on screen.

## Step 5: Handle window resizing

If the user resizes the window, the viewport needs to update too. GLFW lets you register a callback for this:

```cpp
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// after window creation:
glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
```

## Step 6: The render loop

Everything that happens every frame goes in a loop that keeps running until the window is told to close:

```cpp
while (!glfwWindowShouldClose(window)) {
    processInput(window);

    // rendering commands go here

    glfwSwapBuffers(window);
    glfwPollEvents();
}
```

- `glfwSwapBuffers` — GLFW (like most windowing systems) uses **double buffering**: one frame is being drawn while the other is shown, then they swap. This avoids visible tearing/flickering.
- `glfwPollEvents` — checks for input events (keyboard, mouse, window) and calls any registered callbacks.

## Step 7: Basic input handling

```cpp
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
```

## Step 8: Cleanup

```cpp
glfwTerminate();
```

Always release GLFW's resources before the program exits.

## Minimal complete skeleton

```cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Physics Engine", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
```

Next: lesson3p1 adds actual rendering output (clearing the screen to a color) and introduces the graphics pipeline conceptually.
