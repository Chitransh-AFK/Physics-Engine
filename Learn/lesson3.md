# Lesson 3: Organizing OpenGL Code with Object-Oriented Design

## What We've Learned So Far (Review from Lesson 2)

In **Lesson 2**, we learned:
- How to create a **Shader Program** by loading vertex and fragment shaders from files
- How to pass data to the GPU using **Vertex Buffer Objects (VBO)**
- How to use **Vertex Array Objects (VAO)** to describe the layout of vertex data
- How to draw geometry using **glDrawArrays**

We created a simple triangle, but our code was getting a bit messy with raw OpenGL calls scattered throughout `main()`.

---

## What We're Learning Today (Lesson 3)

Today, we're taking the next step: **organizing our code using Object-Oriented Programming (OOP)**. Instead of writing raw OpenGL calls everywhere, we'll create **classes** that wrap common OpenGL operations. This makes our code cleaner, more reusable, and much easier to understand.

We'll also learn about **Element Buffer Objects (EBO)** - a powerful tool for avoiding duplicate vertex data.

---

## The Problem: Duplicate Vertices

Imagine you're drawing a square using two triangles. Here's how it looks:

```
Triangle 1: vertices 0, 1, 2
Triangle 2: vertices 1, 3, 2
```

Without index buffers, you'd have to store vertex data like this:

```cpp
float vertices[] = {
    // Triangle 1
    -0.5f, -0.5f, 0.0f,   // vertex 0
     0.5f, -0.5f, 0.0f,   // vertex 1
     0.0f,  0.5f, 0.0f,   // vertex 2
    
    // Triangle 2
     0.5f, -0.5f, 0.0f,   // vertex 1 (DUPLICATE!)
     0.5f,  0.5f, 0.0f,   // vertex 3
     0.0f,  0.5f, 0.0f    // vertex 2 (DUPLICATE!)
};
```

Notice how vertices 1 and 2 appear twice! This wastes memory.

---

## The Solution: Index Buffers (EBO)

An **Element Buffer Object (EBO)**, also called an Index Buffer, solves this problem. Instead of storing duplicate vertices, we store each unique vertex **once** and use **indices** to reference them.

Here's the efficient approach:

```cpp
float vertices[] = {
    -0.5f, -0.5f, 0.0f,   // index 0
     0.5f, -0.5f, 0.0f,   // index 1
     0.0f,  0.5f, 0.0f,   // index 2
     0.5f,  0.5f, 0.0f    // index 3
};

unsigned int indices[] = {
    0, 1, 2,   // Triangle 1: uses vertices 0, 1, 2
    1, 3, 2    // Triangle 2: uses vertices 1, 3, 2
};
```

Now vertices are stored only once, and we reference them by index!

---

## How EBO Works in Code

### Step 1: Create the EBO Class

We create a class that wraps EBO operations:

```cpp
class EBO {
public:
    GLuint ID;
    
    // Constructor: creates buffer and fills it with data
    EBO(GLuint* indices, GLsizeiptr size) {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
    }
    
    void Bind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    }
    
    void Unbind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    
    void Delete() {
        glDeleteBuffers(1, &ID);
    }
};
```

### Step 2: Use EBO in Your Code

```cpp
GLuint indices[] = {
    0, 3, 5,   // lower left triangle
    3, 2, 4,   // upper triangle
    5, 4, 1    // lower right triangle
};

EBO ebo(indices, sizeof(indices));

// Later when drawing:
glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
```

---

## Object-Oriented Organization: Creating Classes

Instead of scattered OpenGL calls, we create **classes** for each major component:

### 1. Shader Class
Handles loading and compiling shaders:

```cpp
class Shader {
public:
    GLuint ID;
    
    Shader(const char* vertexFile, const char* fragmentFile);
    void Activate();
    void Delete();
};
```

**Benefits:**
- Load shaders from files automatically
- Compile and link in one call
- Easy error handling

### 2. VBO Class
Manages vertex data:

```cpp
class VBO {
public:
    GLuint ID;
    
    VBO(GLfloat* vertices, GLsizeiptr size);
    void Bind();
    void Unbind();
    void Delete();
};
```

### 3. VAO Class
Manages vertex array configuration:

```cpp
class VAO {
public:
    GLuint ID;
    
    VAO();
    void LinkVBO(VBO& vbo, GLuint layout);
    void Bind();
    void Unbind();
    void Delete();
};
```

### 4. EBO Class
Manages index data (as we saw above)

---

## Clean Main Function

With classes, our main function becomes much cleaner:

```cpp
int main() {
    // Initialize OpenGL...
    
    // Create vertices and indices
    float vertices[] = { /* ... */ };
    unsigned int indices[] = { /* ... */ };
    
    // Create objects (one line each!)
    Shader shader("vertex.glsl", "fragment.glsl");
    VAO vao;
    VBO vbo(vertices, sizeof(vertices));
    EBO ebo(indices, sizeof(indices));
    
    // Link everything together
    vao.Bind();
    vao.LinkVBO(vbo, 0);
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
    
    // Render loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        
        shader.Activate();
        vao.Bind();
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Clean up
    vao.Delete();
    vbo.Delete();
    ebo.Delete();
    shader.Delete();
    
    return 0;
}
```

Compare this to raw OpenGL calls scattered everywhere - it's **much more readable**!

---

## Key Best Practices

### 1. Binding Order Matters
When setting up a VAO, the binding order is critical:

```cpp
vao.Bind();                    // Bind VAO FIRST
vbo.Bind();                    // Then bind VBO
glVertexAttribPointer(...);    // Configure vertex attributes
vao.Unbind();                  // Unbind VAO (preserves VBO connection)
vbo.Unbind();                  // Then unbind VBO
```

**Important:** When you unbind a VAO, it "remembers" which VBO was bound. This is why we unbind the VAO first.

### 2. EBO Must Be Bound to VAO
The EBO binding is different - it must be bound to the VAO:

```cpp
vao.Bind();
ebo.Bind();        // EBO binding is part of VAO state
vao.Unbind();      // Unbinding VAO also unbinds EBO
```

### 3. Always Clean Up
Delete buffers when done:

```cpp
vao.Delete();
vbo.Delete();
ebo.Delete();
shader.Delete();
```

---

## Why Use Classes? (The Benefits)

1. **Reusability:** Create VBO, VAO, EBO, or Shader objects anywhere
2. **Encapsulation:** Hide OpenGL complexity behind simple interfaces
3. **Maintainability:** If OpenGL API changes, update one class instead of many files
4. **Readability:** Code intent is clear (what `Shader shader(...)` does is obvious)
5. **Error Handling:** Handle errors in one place
6. **Debugging:** Easier to trace issues to specific components

---

## Summary: What Changed From Lesson 2?

| Aspect | Lesson 2 | Lesson 3 |
|--------|----------|----------|
| **Code Organization** | Raw OpenGL calls | Object-oriented classes |
| **Data Duplication** | All vertex data (some duplicate) | Only unique vertices |
| **Drawing Method** | `glDrawArrays()` | `glDrawElements()` with EBO |
| **Buffer Management** | Manual binding/unbinding | Classes handle it |
| **File Structure** | Everything in main.cpp | Separate .h and .cpp files |
| **Scalability** | Hard to expand | Easy to add more objects |

---

## Quick Reference: Drawing with EBO

```cpp
// 1. Setup
Shader shader("vertex.glsl", "fragment.glsl");
VAO vao;
VBO vbo(vertices, sizeof(vertices));
EBO ebo(indices, sizeof(indices));

vao.Bind();
vao.LinkVBO(vbo, 0);

// 2. Render
shader.Activate();
vao.Bind();
glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

// 3. Cleanup
vao.Delete();
vbo.Delete();
ebo.Delete();
shader.Delete();
```

That's it! Clean, organized, and professional.
