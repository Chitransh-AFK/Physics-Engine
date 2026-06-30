// VAO.cpp - Vertex Array Object implementation
#include "VAO.h"

// Link a VBO into this VAO at the given attribute layout slot.
//
// Assumes the attributes are 3-component floats (vec3) with no padding
// (stride = 3 * sizeof(float)).  All physics-engine shapes use xyz-only
// vertex data, so this single layout covers every mesh.
//
// The VBO is temporarily bound during the call and unbound afterwards;
// the attribute pointer is recorded in the VAO's internal state.
void VAO::LinkVBO(VBO& VBO, GLuint layout) {
    VBO.Bind();
    glVertexAttribPointer(layout,        // shader attribute index
                          3,             // number of components (x, y, z)
                          GL_FLOAT,      // component type
                          GL_FALSE,      // normalise? no
                          3*sizeof(float), // stride (tightly packed)
                          (void*)0);     // offset into buffer
    glEnableVertexAttribArray(layout);
    VBO.Unbind();
}

void VAO::Bind() {
    glBindVertexArray(ID);
}

void VAO::Unbind() {
    glBindVertexArray(0);
}

// Delete the GPU VAO.
// Guards against double-delete and resets ID to 0 afterwards.
void VAO::Delete() {
    if (ID != 0) {
        glDeleteVertexArrays(1, &ID);
        ID = 0;
    }
}