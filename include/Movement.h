#pragma once

#include <GLFW/glfw3.h>

// MovementController encapsulates camera movement and mouse control logic.
// It handles keyboard movement, mouse look, cursor toggling, and view matrix updates.
class MovementController {
public:
    explicit MovementController(GLFWwindow* window);
    ~MovementController() = default;

    // Advance the movement controller state for the current frame.
    void Update();

    // Return the latest view matrix computed from the camera position and direction.
    const float* GetViewMatrix() const;

    // Return the camera position in world space.
    const float* GetCameraPosition() const;

    // Return the camera forward direction.
    const float* GetCameraFront() const;

    // Return the camera up direction.
    const float* GetCameraUp() const;

    // Returns true when the cursor is detached for external usage.
    bool IsCursorDetached() const;

private:
    void ProcessKeyboard();
    void ProcessMouse(double xpos, double ypos);
    void ToggleCursor(GLFWwindow* window);
    void DetachCursor(GLFWwindow* window);
    void AttachCursor(GLFWwindow* window);
    void UpdateDirection();
    void UpdateViewMatrix();

    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static MovementController* s_instance;

    GLFWwindow* m_window;
    float m_cameraPos[3];
    float m_cameraFront[3];
    float m_cameraUp[3];
    float m_cameraYaw;
    float m_cameraPitch;
    float m_lastX;
    float m_lastY;
    bool m_firstMouse;
    bool m_cursorDetached;
    float m_moveSpeed;
    float m_mouseSensitivity;
    float m_lastFrame;
    float m_deltaTime;
    float m_viewMatrix[16];
};
