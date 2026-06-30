// EBO.cpp - Element Buffer Object implementation
#include <EBO.h>

// Create a GL_ELEMENT_ARRAY_BUFFER and optionally upload index data.
// The EBO must be bound while a VAO is active so that the VAO records
// the index buffer binding as part of its state.
EBO::EBO(GLuint* indices, GLsizeiptr size) {
    glGenBuffers(1, &ID);
    if (indices != nullptr && size > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
        // GL_STATIC_DRAW: indices are written once and read many times.
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
    }
}

void EBO::Bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void EBO::Unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Delete the underlying GPU buffer.
// Guards against double-delete (ID=0 check) and resets ID to 0.
void EBO::Delete() {
    if (ID != 0) {
        glDeleteBuffers(1, &ID);
        ID = 0;
    }
}
