# Lesson 3 (Part 2): Vertex Buffer Objects, Shaders, and Drawing the Triangle

> Original study notes for the *Physics_Engine* learning journal.
> Reference: https://learnopengl.com/Getting-started/Hello-Window (not reproduced — concepts explained independently)
> Continues directly from lesson3p1.md, starting at the Vertex Buffer Object.

## Vertex Buffer Objects (VBO)

Sending vertex data to the GPU one vertex at a time would be far too slow — GPU communication has overhead, so data should be sent in large batches. A **Vertex Buffer Object** is GPU-side memory that stores exactly that batch.

Following the object lifecycle pattern from lesson 1 (generate → bind → configure):

```cpp
unsigned int VBO;
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
```

- `glGenBuffers` reserves a buffer ID.
- `glBindBuffer(GL_ARRAY_BUFFER, VBO)` makes this buffer the "current" `GL_ARRAY_BUFFER` in the state machine — any buffer call afterward targeting `GL_ARRAY_BUFFER` affects *this* buffer.
- `glBufferData` actually copies `vertices` from CPU RAM into GPU memory. The last argument is a usage hint telling the driver how to optimize storage:
  - `GL_STATIC_DRAW` — data set once, used many times (a static mesh).
  - `GL_DYNAMIC_DRAW` — data changes occasionally.
  - `GL_STREAM_DRAW` — data changes on nearly every draw call.

## Writing a vertex shader

Shaders are written in **GLSL**, compiled at runtime by the driver. A minimal vertex shader that just passes the position through unmodified:

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos, 1.0);
}
```

`layout (location = 0)` matches the attribute index used later when describing the vertex layout — this is the link between raw buffer bytes and the shader's input variable.

Compiling it at runtime:

```cpp
unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
glCompileShader(vertexShader);

int success;
char infoLog[512];
glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
if (!success) {
    glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
    std::cerr << "Vertex shader compile failed:\n" << infoLog << "\n";
}
```

Always check `GL_COMPILE_STATUS` — GLSL errors fail silently otherwise.

## Writing a fragment shader

```glsl
#version 330 core
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f); // solid orange
}
```

Compiled the same way as the vertex shader, just with `GL_FRAGMENT_SHADER`.

## Linking the shader program

A **shader program** links a vertex shader and fragment shader into one pipeline object that can be activated for drawing:

```cpp
unsigned int shaderProgram = glCreateProgram();
glAttachShader(shaderProgram, vertexShader);
glAttachShader(shaderProgram, fragmentShader);
glLinkProgram(shaderProgram);

glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
    std::cerr << "Shader link failed:\n" << infoLog << "\n";
}

glDeleteShader(vertexShader);
glDeleteShader(fragmentShader);
```

Once linked, the individual shader objects can be deleted — only the linked program is needed from here on.

## Vertex Array Objects (VAO) and attribute layout

A VBO is just raw bytes — OpenGL has no idea those bytes represent groups of 3 floats per vertex until you tell it. A **Vertex Array Object** stores that layout description so it doesn't need to be re-specified on every draw call:

```cpp
unsigned int VAO;
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);

glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
```

`glVertexAttribPointer` arguments, in order:
1. attribute location (`0`, matching `layout (location = 0)` in the shader)
2. number of components per vertex (`3` for x, y, z)
3. data type (`GL_FLOAT`)
4. whether to normalize integer data (irrelevant here, `GL_FALSE`)
5. **stride** — byte distance between consecutive vertices
6. **offset** — byte offset where this attribute starts within a vertex

`glEnableVertexAttribArray(0)` turns the attribute on — without this, the shader won't actually receive the data even if the pointer is correctly configured.

## Drawing the triangle

Inside the render loop, replacing the background-only clear from lesson3p1.md:

```cpp
glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
glClear(GL_COLOR_BUFFER_BIT);

glUseProgram(shaderProgram);
glBindVertexArray(VAO);
glDrawArrays(GL_TRIANGLES, 0, 3);

glfwSwapBuffers(window);
glfwPollEvents();
```

`glDrawArrays(GL_TRIANGLES, 0, 3)` tells OpenGL: starting at vertex 0, take 3 vertices and assemble them into a triangle, running the whole pipeline from lesson3p1.md on them.

## Cleanup

```cpp
glDeleteVertexArrays(1, &VAO);
glDeleteBuffers(1, &VBO);
glDeleteProgram(shaderProgram);
```

At this point the window should render a solid orange triangle on a teal-gray background — the first real rendered output of the engine, and the foundation every later mesh-rendering feature builds on.
