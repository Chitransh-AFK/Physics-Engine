#pragma once

#include <vector>
#include <string>
#include "shaderClass.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include "Movement.h"
#include "GravityController.h"
#include "RotationController.h"

// Enumeration of all supported shape types.
enum class ShapeType {
    Cube,
    Sphere,
    Cylinder,
    Capsule
};

// Base class for all physics shapes.
// Handles GPU mesh data, gravity, picking/dragging, rigid body rotation,
// and angular velocity from off-center grabs.
class Shape {
public:
    // Default constructor (creates an empty, uninitialised shape)
    Shape();

    // Full constructor: uploads mesh to GPU immediately.
    Shape(const std::vector<float>& vertices,
          const std::vector<unsigned int>& indices,
          float r, float g, float b, float a = 1.0f);

    virtual ~Shape();

    // Re-upload mesh to GPU after geometry has been generated.
    void InitializeMesh(const std::vector<float>& vertices,
                        const std::vector<unsigned int>& indices,
                        float r, float g, float b, float a = 1.0f);

    // --- Physics control ---
    void EnableGravity(bool enabled = true);
    void SetRigidBody(bool enabled);
    void SetSize(float size);
    void SetPosition(float x, float y, float z);
    void SetColor(float r, float g, float b, float a = 1.0f);
    void ResetVelocity();

    // --- Accessors for UI panels ---
    bool IsGravityEnabled() const { return m_gravityEnabled; }
    bool IsRigidBody()      const { return m_rigidBodyEnabled; }
    float GetSize()         const { return m_size; }
    const float* GetPosition() const { return m_position; }
    const float* GetColor()    const { return m_color; }
    virtual ShapeType GetShapeType() const = 0;
    virtual std::string GetShapeName() const = 0;

    // --- Per-frame update and rendering ---
    void Update(float deltaTime);
    void Display(Shader& shader, const float viewProjection[16]);

    // --- Cursor picking / dragging ---
    // Test intersection without activating drag. Returns tHit >= 0 on hit, or -1.
    float TestIntersect(const float rayOrigin[3], const float rayDir[3]) const;
    bool TryPick(const float rayOrigin[3], const float rayDir[3],
                 const float camFront[3], double cursorX, double cursorY);
    void UpdateDrag(const float rayOrigin[3], const float rayDir[3],
                    double cursorX, double cursorY);
    void ReleaseDrag();
    bool IsDragging() const;

    // Adjust the floor Y that gravity rests the shape at (call when size changes too).
    void SetFloorY(float y) { m_gravityController.SetFloorY(y); }

    // Legacy aliases kept for compatibility
    void EnableCursor(bool enabled = true) { m_cursorEnabled = enabled; }
    void enableGrav()  { EnableGravity(); }
    void enableCursor(){ EnableCursor(); }
    void display(Shader& shader, const float vp[16]) { Display(shader, vp); }

protected:
    // Sub-classes implement ray intersection for their own shape.
    virtual bool Intersect(const float rayOrigin[3], const float rayDir[3], float& tHit) const = 0;

    // Sub-classes may override to apply extra drag rotation (rigid body).
    virtual void OnDragRotation(float deltaX, float deltaY, float grabOffsetX, float grabOffsetY, float grabOffsetZ) {}

    // Build the full model matrix (translation * rotation * scale).
    void BuildModelMatrix(float out[16]) const;

    // Static ray utility helpers shared by all shapes.
    static bool RayPlaneIntersect(const float origin[3], const float dir[3],
                                   const float planeNormal[3], const float planePoint[3], float& tHit);
    static bool RayIntersectsAABB(const float origin[3], const float dir[3],
                                   const float min[3], const float max[3], float& tHit);
    static bool RayIntersectsSphere(const float origin[3], const float dir[3],
                                     const float center[3], float radius, float& tHit);

    // GPU resources
    std::vector<float>        m_vertices;
    std::vector<unsigned int> m_indices;
    VBO m_vbo;
    EBO m_ebo;
    VAO m_vao;
    unsigned int m_indexCount;

    // Transform
    float m_position[3];
    float m_size;

    // Rendering
    float m_color[4];

    // Physics flags
    bool m_gravityEnabled;
    bool m_cursorEnabled;
    bool m_rigidBodyEnabled;
    bool m_dragging;

    // Drag state
    float m_dragPlaneNormal[3];
    float m_dragPlanePoint[3];
    float m_grabOffset[3];      // World-space offset from centre at pick moment
    float m_lastCursorX;
    float m_lastCursorY;

    // Controllers
    GravityController  m_gravityController;
    RotationController m_rotation;

    // Rigid body angular velocity (radians/s per axis)
    float m_angularVelocity[3];
};

// -------------------------------------------------------------------------
class Cube : public Shape {
public:
    Cube();
    ShapeType   GetShapeType() const override { return ShapeType::Cube; }
    std::string GetShapeName() const override { return "Cube"; }

protected:
    bool Intersect(const float rayOrigin[3], const float rayDir[3], float& tHit) const override;
    void OnDragRotation(float deltaX, float deltaY, float gox, float goy, float goz) override;
};

// -------------------------------------------------------------------------
class Sphere : public Shape {
public:
    Sphere(float radius = 0.5f, int sectors = 24, int stacks = 16);
    ShapeType   GetShapeType() const override { return ShapeType::Sphere; }
    std::string GetShapeName() const override { return "Sphere"; }

protected:
    bool Intersect(const float rayOrigin[3], const float rayDir[3], float& tHit) const override;
    void OnDragRotation(float deltaX, float deltaY, float gox, float goy, float goz) override;

private:
    float m_radius;
    void BuildSphereMesh(float radius, int sectors, int stacks);
};

// -------------------------------------------------------------------------
class Cylinder : public Shape {
public:
    Cylinder(float radius = 0.5f, float height = 1.0f, int sectors = 24);
    ShapeType   GetShapeType() const override { return ShapeType::Cylinder; }
    std::string GetShapeName() const override { return "Cylinder"; }

protected:
    bool Intersect(const float rayOrigin[3], const float rayDir[3], float& tHit) const override;
    void OnDragRotation(float deltaX, float deltaY, float gox, float goy, float goz) override;

private:
    float m_radius;
    float m_height;
    void BuildCylinderMesh(float radius, float height, int sectors);
};

// -------------------------------------------------------------------------
class Capsule : public Shape {
public:
    Capsule(float radius = 0.3f, float height = 1.0f, int sectors = 24, int stacks = 8);
    ShapeType   GetShapeType() const override { return ShapeType::Capsule; }
    std::string GetShapeName() const override { return "Capsule"; }

protected:
    bool Intersect(const float rayOrigin[3], const float rayDir[3], float& tHit) const override;
    void OnDragRotation(float deltaX, float deltaY, float gox, float goy, float goz) override;

private:
    float m_radius;
    float m_height;
    void BuildCapsuleMesh(float radius, float height, int sectors, int stacks);
};
