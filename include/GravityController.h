// ============================================================
// GravityController.h
//
// Simple per-shape gravity simulation.
//
// Each shape owns one GravityController.  Every frame the owner
// passes its Y position by reference to UpdatePosition(), which
// applies a constant downward acceleration (default -9.81 m/s²)
// and clamps the shape to a floor at a configurable Y value.
//
// The floor Y is set per shape type so that each shape's *centre*
// sits at the correct height above Y=0 (the ground plane):
//   Cube     floorY = halfSize  (default 0.5)
//   Sphere   floorY = radius    (default 0.5)
//   Cylinder floorY = halfHeight (default 0.6)
//   Capsule  floorY = halfHeight + radius (default 0.8)
// ============================================================
#pragma once

class GravityController {
public:
    // gravity : downward acceleration in world units/s²  (negative = down)
    // floorY  : Y value the shape centre rests at when on the ground
    explicit GravityController(float gravity = -9.81f, float floorY = 0.5f);

    void   ToggleEnabled();              // flip enabled / disabled
    void   SetEnabled(bool enabled);     // set explicitly; also clears velocity when disabling
    bool   IsEnabled() const;

    void   ResetVelocity();             // zero out vertical velocity (stop mid-air movement)

    // Apply gravity for one timestep and clamp to the floor.
    // |y| is the shape's world-space Y centre position (modified in place).
    void   UpdatePosition(float& y, float deltaTime);

    // Allow changing gravity strength at runtime (e.g. from UI slider).
    void   SetGravity(float g)  { m_gravity = g; }
    float  GetGravity()   const { return m_gravity; }

    // Set the Y position the shape's centre rests at when on the floor.
    // Should equal the shape's half-extent in Y accounting for the current scale.
    void   SetFloorY(float y)   { m_floorY  = y; }

private:
    float m_gravity;    // acceleration (m/s² - typically -9.81)
    float m_floorY;     // minimum allowed Y for the shape centre
    float m_velocity;   // current vertical velocity (m/s)
    bool  m_enabled;    // when false, UpdatePosition is a no-op
};
