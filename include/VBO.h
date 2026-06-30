// ============================================================
// VBO.h - Vertex Buffer Object wrapper
//
// Wraps an OpenGL VBO (GL_ARRAY_BUFFER) that holds vertex data
// (positions, normals, UVs, etc.).
//
// A VBO stores raw float data on the GPU.  It must be linked to
// a VAO via VAO::LinkVBO() before being used in a draw call.
// ============================================================
#ifndef VBO_CLASS_H
#define VBO_CLASS_H
#include <glad/glad.h>

class VBO {
public:
    GLuint ID;   // OpenGL buffer object name (0 = invalid / not yet created)

    // Default constructor: leaves ID = 0 (no GPU allocation).
    // Required so VBO can be a default-initialised member of Shape.
    VBO() : ID(0) {}

    // Create a GPU buffer and upload |size| bytes from |vertices|.
    // If vertices is nullptr or size is 0, the buffer is created but
    // left empty (data can be uploaded later).
    VBO(GLfloat* vertices, GLsizeiptr size);

    void Bind();    // bind as GL_ARRAY_BUFFER (makes this the active VBO)
    void Unbind();  // unbind (restore default binding 0)

    // Delete the GPU buffer.  Safe to call on an invalid (ID=0) VBO;
    // also resets ID to 0 so double-delete is harmless.
    void Delete();
};

#endif