// ============================================================
// RotationController.h
//
// Manages object or camera orientation using Euler angles (yaw, pitch).
// Builds 4x4 orientation and model transformation matrices.
// Prevents gimbal lock / flipping by clamping pitch to [-89, 89] degrees.
// ============================================================
#pragma once

class RotationController {
public:
    // sensitivity : speed scale of yaw/pitch adjustment per unit delta
    explicit RotationController(float sensitivity = 0.005f);

    // Apply incremental cursor offsets (e.g. from mouse dragging) to yaw and pitch
    void AddCursorDelta(float deltaX, float deltaY);

    // Compute a pure 4x4 rotation matrix representing the current orientation
    void GetRotationMatrix(float out[16]) const;

    // Build a combined model matrix: Translation * Rotation
    void BuildModelMatrix(float x, float y, float z, float out[16]) const;

    // Getters for yaw and pitch
    float GetYaw() const;
    float GetPitch() const;

private:
    float m_yaw;          // Rotation around the Y axis (yaw, in degrees)
    float m_pitch;        // Rotation around the X axis (pitch, in degrees)
    float m_sensitivity;  // Mouse input sensitivity factor
};
