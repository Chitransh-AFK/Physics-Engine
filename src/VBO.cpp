// VBO.cpp - Vertex Buffer Object implementation
#include "VBO.h"

// Create a GL_ARRAY_BUFFER and optionally upload vertex data.
// If vertices == nullptr or size == 0, the buffer is generated but
// glBufferData is NOT called (caller must upload data later).
VBO::VBO(GLfloat* vertices, GLsizeiptr size) {
    glGenBuffers(1, &ID);
    if (vertices != nullptr && size > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, ID);
        // GL_STATIC_DRAW: data is written once and read many times.
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    }
}

void VBO::Bind() {
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::Unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Delete the underlying GPU buffer.
// Guards against double-delete by checking ID != 0 first, then
// resets ID to 0 so subsequent calls are no-ops.
void VBO::Delete() {
    if (ID != 0) {
        glDeleteBuffers(1, &ID);
        ID = 0;
    }
}
