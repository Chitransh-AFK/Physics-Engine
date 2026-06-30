// RotationController.cpp - orientation state and transformation builder
#include "RotationController.h"
#include "MathUtils.h"

RotationController::RotationController(float sensitivity)
    : m_yaw(0.0f),
      m_pitch(0.0f),
      m_sensitivity(sensitivity) {
}

// Add delta inputs (typically from mouse movement or drag gesture) to yaw and pitch.
// Pitch is clamped to prevent the camera or object flipping upside down.
void RotationController::AddCursorDelta(float deltaX, float deltaY) {
    m_yaw += deltaX * m_sensitivity;
    m_pitch += deltaY * m_sensitivity;

    // Clamp pitch between -89 and 89 degrees to avoid gimbal lock and camera flip.
    if (m_pitch > 89.0f) {
        m_pitch = 89.0f;
    }
    if (m_pitch < -89.0f) {
        m_pitch = -89.0f;
    }
}

// Generates a combined 4x4 rotation matrix: RotY(yaw) * RotX(pitch).
// All matrices are column-major, matching OpenGL's format.
void RotationController::GetRotationMatrix(float out[16]) const {
    float rotationX[16];
    float rotationY[16];

    // Convert degrees to radians and construct the axial rotation matrices
    RotateXMat4(rotationX, Radians(m_pitch));
    RotateYMat4(rotationY, Radians(m_yaw));

    // Combine them (Y rotation applied first, then X)
    MultiplyMat4(rotationY, rotationX, out);
}

// Generates a complete model transformation matrix (T * R) to position and orient the shape.
void RotationController::BuildModelMatrix(float x, float y, float z, float out[16]) const {
    float rotation[16];
    GetRotationMatrix(rotation);

    float translation[16];
    TranslateMat4(translation, x, y, z);

    // Apply rotation first, then translation: out = T * R
    MultiplyMat4(translation, rotation, out);
}

float RotationController::GetYaw() const {
    return m_yaw;
}

float RotationController::GetPitch() const {
    return m_pitch;
}
