#include "Movement.h"
#include <cmath>
#include <iostream>

MovementController* MovementController::s_instance = nullptr;

// Convert degrees to radians.
static float radians(float degrees) {
    return degrees * 3.14159265358979323846f / 180.0f;
}

// Cross product helper for 3D vectors.
static void cross3(const float a[3], const float b[3], float out[3]) {
    out[0] = a[1] * b[2] - a[2] * b[1];
    out[1] = a[2] * b[0] - a[0] * b[2];
    out[2] = a[0] * b[1] - a[1] * b[0];
}

// Dot product helper for 3D vectors.
static float dot3(const float a[3], const float b[3]) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

// Normalize a 3D vector in place.
static void normalize3(float v[3]) {
    float len = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (len > 0.0f) {
        v[0] /= len;
        v[1] /= len;
        v[2] /= len;
    }
}

static void multiplyMat4(const float a[16], const float b[16], float out[16]) {
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

static void lookAt(float eyeX, float eyeY, float eyeZ,
                   float centerX, float centerY, float centerZ,
                   float upX, float upY, float upZ,
                   float view[16]) {
    float f[3] = {centerX - eyeX, centerY - eyeY, centerZ - eyeZ};
    normalize3(f);
    float up[3] = {upX, upY, upZ};
    normalize3(up);
    float s[3];
    cross3(f, up, s);
    normalize3(s);
    float u[3];
    cross3(s, f, u);

    view[0] = s[0];
    view[1] = u[0];
    view[2] = -f[0];
    view[3] = 0.0f;
    view[4] = s[1];
    view[5] = u[1];
    view[6] = -f[1];
    view[7] = 0.0f;
    view[8] = s[2];
    view[9] = u[2];
    view[10] = -f[2];
    view[11] = 0.0f;
    float eye[3] = {eyeX, eyeY, eyeZ};
    view[12] = -dot3(s, eye);
    view[13] = -dot3(u, eye);
    view[14] = dot3(f, eye);
    view[15] = 1.0f;
}

static void perspective(float fovRadians, float aspect, float nearPlane, float farPlane, float projection[16]) {
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

MovementController::MovementController(GLFWwindow* window)
    : m_window(window),
      m_cameraPos{0.0f, 0.0f, 3.0f},
      m_cameraFront{0.0f, 0.0f, -1.0f},
      m_cameraUp{0.0f, 1.0f, 0.0f},
      m_cameraYaw(-90.0f),
      m_cameraPitch(0.0f),
      m_lastX(400.0f),
      m_lastY(300.0f),
      m_firstMouse(true),
      m_cursorDetached(false),
      m_moveSpeed(2.0f),
      m_mouseSensitivity(0.1f),
      m_lastFrame(0.0f),
      m_deltaTime(0.0f)
{
    s_instance = this;
    glfwSetCursorPosCallback(m_window, MouseCallback);
    glfwSetKeyCallback(m_window, KeyCallback);
    AttachCursor(m_window);
    UpdateViewMatrix();
}

// Toggle between captured and free cursor modes.
// When the cursor is detached, it is visible and can leave the window.
// When attached, the cursor is hidden and used for mouse look.
void MovementController::ToggleCursor(GLFWwindow* window) {
    if (m_cursorDetached) {
        AttachCursor(window);
    } else {
        DetachCursor(window);
    }
}

// Detach the cursor so external applications can use it.
void MovementController::DetachCursor(GLFWwindow* window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    m_cursorDetached = true;
    m_firstMouse = true;
}

// Attach the cursor for mouse look control.
void MovementController::AttachCursor(GLFWwindow* window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    m_cursorDetached = false;
    m_firstMouse = true;
}

void MovementController::UpdateDirection() {
    float yawRad = radians(m_cameraYaw);
    float pitchRad = radians(m_cameraPitch);
    m_cameraFront[0] = std::cos(yawRad) * std::cos(pitchRad);
    m_cameraFront[1] = std::sin(pitchRad);
    m_cameraFront[2] = std::sin(yawRad) * std::cos(pitchRad);
    normalize3(m_cameraFront);
}

void MovementController::UpdateViewMatrix() {
    float target[3] = {
        m_cameraPos[0] + m_cameraFront[0],
        m_cameraPos[1] + m_cameraFront[1],
        m_cameraPos[2] + m_cameraFront[2]
    };
    lookAt(m_cameraPos[0], m_cameraPos[1], m_cameraPos[2],
           target[0], target[1], target[2],
           m_cameraUp[0], m_cameraUp[1], m_cameraUp[2],
           m_viewMatrix);
}

void MovementController::ProcessKeyboard() {
    float currentFrame = static_cast<float>(glfwGetTime());
    m_deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;

    float velocity = m_moveSpeed * m_deltaTime;
    float right[3];
    cross3(m_cameraFront, m_cameraUp, right);
    normalize3(right);

    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
        m_cameraPos[0] += m_cameraFront[0] * velocity;
        m_cameraPos[1] += m_cameraFront[1] * velocity;
        m_cameraPos[2] += m_cameraFront[2] * velocity;
    }
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
        m_cameraPos[0] -= m_cameraFront[0] * velocity;
        m_cameraPos[1] -= m_cameraFront[1] * velocity;
        m_cameraPos[2] -= m_cameraFront[2] * velocity;
    }
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
        m_cameraPos[0] -= right[0] * velocity;
        m_cameraPos[1] -= right[1] * velocity;
        m_cameraPos[2] -= right[2] * velocity;
    }
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
        m_cameraPos[0] += right[0] * velocity;
        m_cameraPos[1] += right[1] * velocity;
        m_cameraPos[2] += right[2] * velocity;
    }
    if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        m_cameraPos[1] += velocity;
    }
    if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        m_cameraPos[1] -= velocity;
    }
}

// Handle raw mouse movement and update camera orientation.
// Mouse input is ignored while the cursor is detached.
void MovementController::ProcessMouse(double xpos, double ypos) {
    if (m_cursorDetached) {
        return;
    }
    if (m_firstMouse) {
        m_lastX = static_cast<float>(xpos);
        m_lastY = static_cast<float>(ypos);
        m_firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - m_lastX;
    float yoffset = m_lastY - static_cast<float>(ypos);
    m_lastX = static_cast<float>(xpos);
    m_lastY = static_cast<float>(ypos);

    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_cameraYaw += xoffset;
    m_cameraPitch += yoffset;

    if (m_cameraPitch > 89.0f) m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    UpdateDirection();
}

void MovementController::Update() {
    ProcessKeyboard();
    UpdateViewMatrix();
}

const float* MovementController::GetViewMatrix() const {
    return m_viewMatrix;
}

const float* MovementController::GetCameraPosition() const {
    return m_cameraPos;
}

const float* MovementController::GetCameraFront() const {
    return m_cameraFront;
}

const float* MovementController::GetCameraUp() const {
    return m_cameraUp;
}

bool MovementController::IsCursorDetached() const {
    return m_cursorDetached;
}

void MovementController::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (s_instance) {
        s_instance->ProcessMouse(xpos, ypos);
    }
}

void MovementController::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS && s_instance) {
        s_instance->ToggleCursor(window);
    }
}
