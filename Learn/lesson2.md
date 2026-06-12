# Lesson 2: OpenGL Rendering Pipeline and Shader Workflow

If you finished Lesson 1, you already know how to open a window and keep it running. In Lesson 2, we will use that window to draw a triangle with OpenGL.

This lesson is organized like this:
1. theory first, with beginner-friendly analogies and diagrams
2. code in small pieces
3. explanation immediately after each code block

## 1. Theory: What OpenGL Does and Why

OpenGL is a system that helps the CPU talk to the GPU. The CPU sets up the data and the GPU does the heavy work of turning points into pixels.

### The GPU is like a factory line

Think of the GPU as a factory. The raw material is vertex data, and the final product is colored pixels on the screen.

- The CPU is the office worker who writes the instructions.
- The GPU is the factory floor that actually builds the product.
- Shaders are the workers on the factory line: each worker has a specific job.

The main stages are:

    CPU (your program) -> GPU (factory) -> Screen

### Diagram of the pipeline

```
[Vertex Data] -> [Vertex Shader] -> [Shape Assembler] -> [Rasterizer] -> [Fragment Shader] -> [Framebuffer]
```

### What is a vertex?

A **vertex** is a single point. If you want to draw a shape, you describe it using vertices.

Example:
- 1 point = 1 vertex
- 3 points = 1 triangle

A vertex usually contains:
- position: where the point is in space
- optional extra values: color, normals, texture coordinates

### Why the GPU is better at this

The GPU is built to work on many points at once. If you have 3 vertices for a triangle, the GPU can process them in parallel instead of one by one.

That is why we send data to the GPU and let it do the heavy lifting.

## 2. Theory: The shader stages

### Vertex Shader

The vertex shader is the first worker in the factory line.

It takes each vertex and decides where that vertex should appear on the screen.

Real-life analogy:
- You have a map with coordinates.
- The vertex shader is the worker who reads a coordinate and points to the correct location on the board.

On the GPU, this happens for every vertex.

### Shape Assembler

Once the GPU knows the positions, it groups vertices into shapes.

Example:
- vertex 1 + vertex 2 + vertex 3 = triangle

The GPU now knows which three points make one triangle.

### Rasterization

This stage fills the shape with fragments.

Analogy:
- You drew the outline of a triangle on paper.
- Rasterization colors the inside of the triangle with tiny dots.

Each dot is a fragment. The GPU decides which dots are inside the shape.

### Fragment Shader

The fragment shader colors each fragment.

Analogy:
- The fragment shader is the painter.
- It decides the final color for every tiny dot inside the triangle.

The GPU runs the fragment shader on each fragment in parallel.

## 3. Theory: Buffers and memory

### Vertex Buffer Object (VBO)

A VBO is where vertex data lives on the GPU.

Analogy:
- The CPU writes the recipe on paper.
- The VBO puts the recipe in the factory kitchen.

Once the vertex data is in the VBO, the GPU can read it directly.

### Vertex Array Object (VAO)

A VAO remembers how to read the VBO.

Analogy:
- The VBO is the ingredients.
- The VAO is the instructions that say how to use them.

The VAO tells OpenGL:
- what data belongs to each vertex
- how many values are in each vertex
- where each vertex starts inside the buffer

## 4. Code example with line-by-line explanation

The code below is written to match the theory above. After each code block, the explanation explains what the code does and how the GPU uses it.

### Code: includes and shader sources

```cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

const char* vertexShaderSource = R"glsl(
#version 330 core
layout(location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
}
)glsl";
```

This block does three things:
1. Includes `glad` and `GLFW` so OpenGL functions and window creation are available.
2. Defines the vertex shader code as `vertexShaderSource`.
3. Defines the fragment shader code as `fragmentShaderSource`.

The GPU will later compile these strings into shader programs.

### Code: initialize GLFW and create a window

```cpp
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Triangle", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
```

Explanation:
- `glfwInit()` starts GLFW, which creates windows and handles input.
- `glfwWindowHint(...)` requests an OpenGL 3.3 core profile context.
- `glfwCreateWindow(...)` opens a window of size 800x600.
- `glfwMakeContextCurrent(window)` makes this window the active OpenGL target.
- `gladLoadGLLoader(...)` loads the GPU-specific OpenGL functions.

At this point, the GPU context exists, and OpenGL calls can be made.

### Code: define triangle vertices

```cpp
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
```

Explanation:
- This array defines three vertices.
- Each vertex has `x`, `y`, and `z` values.
- The `z` value is 0, so the triangle is flat in 2D.

These coordinates are in normalized device coordinates (NDC), where `-1.0` is left or bottom and `1.0` is right or top.

### Code: create VAO and VBO

```cpp
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
```

Explanation:
- `glGenVertexArrays(1, &VAO)` creates one VAO.
- `glGenBuffers(1, &VBO)` creates one VBO.
- `glBindVertexArray(VAO)` makes the VAO active. The following setup is stored inside it.
- `glBindBuffer(GL_ARRAY_BUFFER, VBO)` makes the VBO active.
- `glBufferData(...)` copies the vertex array from CPU memory to GPU memory.
- `glVertexAttribPointer(...)` tells OpenGL how to read the vertex data:
  - index `0` means this is vertex attribute location 0.
  - `3` means three numbers per vertex.
  - `GL_FLOAT` means each number is a float.
  - `3 * sizeof(float)` is the stride, the size of one vertex.
  - `(void*)0` means the data starts at the beginning of the buffer.
- `glEnableVertexAttribArray(0)` enables the vertex attribute so the GPU can use it.
- `glBindBuffer(GL_ARRAY_BUFFER, 0)` and `glBindVertexArray(0)` deactivate the current buffer and VAO.

On the GPU:
- the VBO now stores the triangle vertex positions.
- the VAO remembers how those positions map to the vertex shader input.

### Code: compile vertex shader

```cpp
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
```

Explanation:
- `glCreateShader(GL_VERTEX_SHADER)` makes a shader object for the vertex shader.
- `glShaderSource(...)` gives the shader its GLSL source code.
- `glCompileShader(vertexShader)` compiles the code into GPU machine instructions.

If the code contains errors, this step would fail and you would read the error log.

### Code: compile fragment shader

```cpp
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
```

Explanation:
- This is the same process as the vertex shader, but for the fragment shader.
- The GPU now has machine code for coloring fragments.

### Code: link the shader program

```cpp
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
```

Explanation:
- `glCreateProgram()` makes a program object that will connect both shaders.
- `glAttachShader(...)` adds the compiled shaders to the program.
- `glLinkProgram(shaderProgram)` links them into one executable pipeline.
- `glDeleteShader(...)` deletes the individual shader objects because they are no longer needed after linking.

At this point, the GPU has a complete pipeline: vertex shading and fragment shading are connected.

### Code: render loop

```cpp
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
```

Explanation:
- `glClearColor(...)` sets the background color.
- `glClear(GL_COLOR_BUFFER_BIT)` fills the screen with that color.
- `glUseProgram(shaderProgram)` tells OpenGL to use the linked shaders.
- `glBindVertexArray(VAO)` tells OpenGL which vertex setup to use.
- `glDrawArrays(GL_TRIANGLES, 0, 3)` draws three vertices as one triangle.
- `glfwSwapBuffers(window)` shows the rendered image.
- `glfwPollEvents()` checks for keyboard, mouse, and window events.

GPU-level view of `glDrawArrays`:
- the GPU reads the vertex positions from the VBO.
- the vertex shader runs for each vertex.
- the GPU assembles a triangle from the three vertices.
- rasterization converts the triangle into fragments.
- the fragment shader runs for each fragment.
- the pixel colors are written to the framebuffer.

### Code: cleanup

```cpp
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

Explanation:
- `glDeleteVertexArrays(...)` frees the VAO.
- `glDeleteBuffers(...)` frees the VBO.
- `glDeleteProgram(...)` frees the shader program.
- `glfwDestroyWindow(window)` closes the window.
- `glfwTerminate()` stops GLFW.

This is good practice because it frees GPU and system resources.

## 5. Summary of what happens

1. The CPU sets up a window and creates shader source strings.
2. The CPU sends vertex data into a VBO on the GPU.
3. The VAO remembers how the GPU should read that vertex data.
4. The GPU compiles the shaders and links them into a shader program.
5. In the render loop, `glDrawArrays` sends the data through the GPU pipeline.
6. The vertex shader moves vertices into screen space.
7. The rasterizer fills the triangle with fragments.
8. The fragment shader colors each fragment.
9. The GPU writes the final pixels to the screen.

## 6. Final analogy

- VBO = ingredients in the kitchen
- VAO = recipe instructions
- Vertex shader = the prep cook who places ingredients
- Rasterizer = the person filling the plate
- Fragment shader = the person seasoning each bite
- Framebuffer = the finished plate served to the customer

That is how OpenGL turns your triangle data into a picture on the screen.

## 7. Step-by-Step Code Example

Below is a complete example showing how to set up shaders, send vertex data to the GPU, and draw a triangle. After the code, there is a line-by-line explanation of what each part is doing.

```cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

const char* vertexShaderSource = R"glsl(
#version 330 core
layout(location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos, 1.0);
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
}
)glsl";

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Triangle", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

### What this code actually does

1. `glfwInit()` and window creation
   - Sets up GLFW and creates a window.
   - The window holds the OpenGL context, which is the connection between your program and the GPU.

2. `gladLoadGLLoader(...)`
   - Loads the OpenGL function pointers for your GPU and driver.
   - Without this, the program cannot call OpenGL functions.

3. Vertex data array
   - Defines three vertices for a triangle.
   - Each vertex has three floats: x, y, and z.
   - This data is still in CPU memory at this point.

4. `glGenVertexArrays` and `glGenBuffers`
   - Creates a VAO and a VBO.
   - The VAO stores how to read vertex data.
   - The VBO stores the raw vertex values on the GPU.

5. `glBindVertexArray(VAO)`
   - Activates the VAO.
   - All subsequent vertex attribute setup is recorded in this VAO.

6. `glBindBuffer(GL_ARRAY_BUFFER, VBO)` and `glBufferData(...)`
   - Binds the VBO as the current array buffer.
   - Copies the vertex data from CPU memory into GPU memory.
   - After this call, the triangle data lives on the GPU.

7. `glVertexAttribPointer(...)` and `glEnableVertexAttribArray(0)`
   - Tells OpenGL how to interpret the vertex data in the VBO.
   - The first attribute (`location = 0`) reads 3 floats per vertex.
   - This matches `layout(location = 0) in vec3 aPos;` in the vertex shader.

8. Shader creation and compilation
   - `glCreateShader` makes a shader object.
   - `glShaderSource` attaches the GLSL source code.
   - `glCompileShader` turns the source into GPU machine code.
   - The GPU driver checks the code and prepares it for execution.

9. Program linking
   - `glCreateProgram` makes a program object.
   - `glAttachShader` adds the compiled vertex and fragment shaders.
   - `glLinkProgram` connects them together.
   - The GPU driver verifies that the vertex shader output and fragment shader input match.

10. Render loop
    - `glClearColor` and `glClear` reset the screen each frame.
    - `glUseProgram` activates the shader program.
    - `glBindVertexArray(VAO)` activates the vertex state.
    - `glDrawArrays(GL_TRIANGLES, 0, 3)` tells the GPU to draw one triangle using the first three vertices.
    - `glfwSwapBuffers` shows the rendered frame on screen.

11. GPU-level work during `glDrawArrays`
    - The GPU reads the vertex positions from the VBO.
    - The vertex shader runs once per vertex and writes `gl_Position`.
    - The assembler groups the vertices into a triangle.
    - Rasterization converts the triangle into fragments.
    - The fragment shader runs once per fragment and writes a color.
    - The final colors are written into the framebuffer.

12. Cleanup
    - `glDeleteVertexArrays`, `glDeleteBuffers`, and `glDeleteProgram` release GPU resources.
    - `glfwDestroyWindow` and `glfwTerminate` close the window and free GLFW resources.

### How it works on a GPU level

- The CPU prepares data and sends commands to the GPU.
- The vertex data is stored in GPU memory inside the VBO.
- The GPU executes the vertex shader in parallel for each vertex.
- After vertices are processed, the GPU assembles triangles.
- The rasterizer converts triangles into fragments and interpolates values.
- The fragment shader runs in parallel for each fragment.
- The GPU writes the finished pixels into the framebuffer.

This example shows the full flow from CPU setup to GPU execution, and it is the easiest way to see how OpenGL draws a triangle.