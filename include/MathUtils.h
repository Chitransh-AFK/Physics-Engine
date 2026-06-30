// ============================================================
// MathUtils.h
//
// Lightweight 4x4 matrix mathematics library.
// All matrices are column-major (OpenGL standard):
//   matrix[col * 4 + row]
//
// Design constraint:
//   Avoids any dependency on external libraries like GLM to keep
//   the physics engine codebase self-contained and easy to build.
// ============================================================
#pragma once

#include <cmath>

// Convert degrees to radians
inline float Radians(float degrees) {
    return degrees * 3.14159265358979323846f / 180.0f;
}

// Generate a 4x4 identity matrix (1s on diagonal, 0s elsewhere)
inline void IdentityMat4(float matrix[16]) {
    for (int i = 0; i < 16; ++i) {
        matrix[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    }
}

// Generate a 4x4 translation matrix
inline void TranslateMat4(float matrix[16], float x, float y, float z) {
    IdentityMat4(matrix);
    matrix[12] = x;
    matrix[13] = y;
    matrix[14] = z;
}

// Generate a 4x4 rotation matrix around the X axis (pitch)
inline void RotateXMat4(float matrix[16], float angleRadians) {
    float c = std::cos(angleRadians);
    float s = std::sin(angleRadians);
    IdentityMat4(matrix);
    matrix[5] = c;
    matrix[6] = s;
    matrix[9] = -s;
    matrix[10] = c;
}

// Generate a 4x4 rotation matrix around the Y axis (yaw)
inline void RotateYMat4(float matrix[16], float angleRadians) {
    float c = std::cos(angleRadians);
    float s = std::sin(angleRadians);
    IdentityMat4(matrix);
    matrix[0] = c;
    matrix[2] = -s;
    matrix[8] = s;
    matrix[10] = c;
}

// Multiplies two 4x4 matrices: out = a * b.
// Columns of 'b' are multiplied by rows of 'a'.
inline void MultiplyMat4(const float a[16], const float b[16], float out[16]) {
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k) {
                sum += a[k * 4 + row] * b[col * 4 + k];
            }
            out[col * 4 + row] = sum;
        }
    }
}

// Build a standard perspective projection matrix (equivalent to glm::perspective).
//   fovRadians : vertical field-of-view
//   aspect     : viewport width / height
//   nearPlane  : distance to near clipping plane
//   farPlane   : distance to far clipping plane
//   projection : output 16-element float array
inline void Perspective(float fovRadians, float aspect, float nearPlane, float farPlane, float projection[16]) {
    float f = 1.0f / std::tan(fovRadians / 2.0f);
    projection[0] = f / aspect;
    projection[1] = 0.0f;
    projection[2] = 0.0f;
    projection[3] = 0.0f;
    projection[4] = 0.0f;
    projection[5] = f;
    projection[6] = 0.0f;
    projection[7] = 0.0f;
    projection[8] = 0.0f;
    projection[9] = 0.0f;
    projection[10] = (farPlane + nearPlane) / (nearPlane - farPlane);
    projection[11] = -1.0f;
    projection[12] = 0.0f;
    projection[13] = 0.0f;
    projection[14] = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
    projection[15] = 0.0f;
}
