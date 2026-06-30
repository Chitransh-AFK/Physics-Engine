// GravityController.cpp - per-shape gravity simulation
#include "GravityController.h"

GravityController::GravityController(float gravity, float floorY)
    : m_gravity(gravity),
      m_floorY(floorY),
      m_velocity(0.0f),
      m_enabled(true) {
}

// Flip the enabled flag without touching velocity.
void GravityController::ToggleEnabled() {
    m_enabled = !m_enabled;
}

// Set enabled state explicitly.
// When disabling, immediately zero the velocity so the shape doesn't
// resume falling if gravity is switched back on later.
void GravityController::SetEnabled(bool enabled) {
    m_enabled = enabled;
    if (!enabled) m_velocity = 0.0f;
}

bool GravityController::IsEnabled() const {
    return m_enabled;
}

// Stop vertical motion (used when a shape is grabbed or reset).
void GravityController::ResetVelocity() {
    m_velocity = 0.0f;
}

// Integrate one physics timestep:
//   velocity += gravity * dt     (Euler integration)
//   y        += velocity * dt
//   if y < floorY  ->  clamp and stop (inelastic floor)
void GravityController::UpdatePosition(float& y, float deltaTime) {
    if (!m_enabled) {
        return;   // gravity disabled: leave y unchanged
    }

    m_velocity += m_gravity * deltaTime;   // accelerate downward
    y          += m_velocity * deltaTime;  // integrate position

    // Floor collision: stop at the resting height for this shape type.
    if (y < m_floorY) {
        y          = m_floorY;
        m_velocity = 0.0f;   // inelastic: no bounce
    }
}
