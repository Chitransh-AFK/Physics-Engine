// ============================================================
// EBO.h - Element Buffer Object wrapper
//
// Wraps an OpenGL EBO (GL_ELEMENT_ARRAY_BUFFER) that stores the
// index list used by glDrawElements().
//
// In OpenGL Core Profile the EBO binding is part of VAO state,
// so the EBO should always be bound WHILE a VAO is bound.
// ============================================================
#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <glad/glad.h>

class EBO {
public:
    GLuint ID;   // OpenGL buffer object name (0 = invalid)

    // Default constructor: leaves ID = 0 (no GPU allocation).
    EBO() : ID(0) {}

    // Create a GPU buffer and upload |size| bytes of index data.
    // If indices is nullptr or size is 0, the buffer is allocated
    // but not filled (data can be supplied later).
    EBO(GLuint* indices, GLsizeiptr size);

    void Bind();    // bind as GL_ELEMENT_ARRAY_BUFFER
    void Unbind();  // restore default binding 0

    // Delete the GPU buffer.  Safe on ID=0; resets ID to 0 afterwards.
    void Delete();
};

#endif