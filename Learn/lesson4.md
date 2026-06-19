# Lesson 4: Abstraction

> Original study notes for the *Physics_Engine* learning journal.
> This lesson explains how we moved raw OpenGL setup into reusable wrapper classes.

## Why abstraction matters

Raw OpenGL code quickly becomes noisy and hard to maintain. Every object requires a sequence of low-level steps:

- generate an OpenGL object ID
- bind the object
- upload data or configure state
- unbind when finished
- delete the object when it is no longer needed

When all of these steps happen directly in `main.cpp`, the program logic gets mixed with GPU management details. Abstraction separates those responsibilities so the main program can focus on what is being rendered, not how OpenGL sets it up.

## What we abstracted

In this project, we moved four responsibilities out of `main.cpp` and into dedicated classes:

- `Shader` — loads, compiles, and links shader programs from external files
- `VBO` — manages a vertex buffer object containing vertex data
- `VAO` — manages a vertex array object and the connection between a VBO and a shader attribute layout
- `EBO` — manages an element buffer object for indexed drawing

This keeps `main.cpp` clean and gives us reusable building blocks for future scenes.

## Before: inline setup in `main.cpp`

The original `main.cpp` contained raw calls like:

```cpp
unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
glCompileShader(vertexShader);
```

and:

```cpp
unsigned int VBO;
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
```

That is useful when learning the pipeline, but it is not ideal for a real engine because every new mesh duplicates the same boilerplate.

## After: using wrapper classes

The refactored `main.cpp` now reads like this:

```cpp
Shader shader("Resources/Shaders/default.vert", "Resources/Shaders/default.frag");
VAO vao;
VBO vbo(vertices, sizeof(vertices));
EBO ebo(indices, sizeof(indices));

vao.Bind();
ebo.Bind();
vao.LinkVBO(vbo, 0);
vao.Unbind();
ebo.Unbind();
```

This code expresses the high-level structure of the render setup without the low-level details. It also makes the initialization order explicit and easier to understand.

## How each abstraction works

### Shader

The `Shader` class performs these tasks:

- read shader source from `.vert` and `.frag` files
- compile the vertex shader
- compile the fragment shader
- link them into a shader program
- expose an `Activate()` method to use the program
- expose a `Delete()` method to free GPU resources

This means the rest of the program never needs to call `glCreateShader`, `glShaderSource`, or `glLinkProgram` directly.

### VBO

The `VBO` class encapsulates vertex buffer creation and data upload:

- `glGenBuffers`
- `glBindBuffer(GL_ARRAY_BUFFER, ...)`
- `glBufferData(...)`

It also provides `Bind()`, `Unbind()`, and `Delete()` helpers.

### VAO

The `VAO` class encapsulates the vertex array object and the vertex attribute setup. When we call `LinkVBO(vbo, 0)`, the class:

- binds the VBO
- sets the vertex attribute pointer for layout location 0
- enables that attribute
- unbinds the VBO again

This captures the common pattern for linking vertex data to shader inputs.

### EBO

The `EBO` class encapsulates indexed drawing setup. It handles:

- `glGenBuffers`
- `glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ...)`
- `glBufferData(...)`

Using an EBO is what allows `glDrawElements` to reuse vertex data instead of duplicating vertices.

## Benefits of this abstraction

- `main.cpp` is easier to read and reason about
- shader code is stored in separate files, making it easier to edit and reuse
- GPU resource management is centralized
- new meshes can be added with much less boilerplate
- the design is closer to a real engine structure

## What to do next

In the next lesson, we can extend this abstraction further by adding:

- a `Texture` class for image data
- a `Mesh` or `Model` class that combines VAO, VBO, and EBO
- a `Camera` class for view/projection transforms

That will let `main.cpp` describe the scene more than the rendering plumbing.
