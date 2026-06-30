// ============================================================
// VAO.h - Vertex Array Object wrapper
//
// Wraps an OpenGL VAO (Vertex Array Object) which records:
//   - Which VBO attributes are active and their layouts
//   - The EBO (element buffer) binding for indexed drawing
//
// In OpenGL 3.3 Core Profile every draw call requires a bound VAO.
// ============================================================
#ifndef VAO_CLASS_H
#define VAO_CLASS_H
#include <glad/glad.h>
#include <VBO.h>

class VAO {
public:
    GLuint ID;   // OpenGL VAO name (0 = invalid)

    // Default constructor: immediately allocates a GPU VAO.
    // Called at construction time so that the VAO is always ready to
    // be bound and configured.
    VAO() : ID(0) {
        glGenVertexArrays(1, &ID);
    }

    // Bind a VBO into this VAO at the given attribute layout index.
    // Assumes 3-component float attributes (xyz) tightly packed.
    void LinkVBO(VBO& VBO, GLuint layout);

    void Bind();    // make this VAO current (glBindVertexArray)
    void Unbind();  // restore default VAO binding (0)

    // Delete the GPU VAO.  Safe on ID=0; resets ID to 0 afterwards.
    void Delete();
};


#endif