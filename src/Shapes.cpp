#include "Shapes.h"
#include "MathUtils.h"
#include <cmath>
#include <cstring>

// ============================================================
// Static math helpers
// ============================================================
static float dot3(const float a[3], const float b[3]) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

static void cross3(const float a[3], const float b[3], float out[3]) {
    out[0] = a[1]*b[2] - a[2]*b[1];
    out[1] = a[2]*b[0] - a[0]*b[2];
    out[2] = a[0]*b[1] - a[1]*b[0];
}

static float len3(const float v[3]) {
    return std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

// ============================================================
// Shape - default constructor
// ============================================================
Shape::Shape()
    : m_vbo(nullptr, 0),
      m_ebo(nullptr, 0),
      m_vao(),
      m_indexCount(0),
      m_size(1.0f),
      m_gravityEnabled(false),
      m_cursorEnabled(true),
      m_rigidBodyEnabled(false),
      m_dragging(false),
      m_gravityController(),
      m_rotation()
{
    m_position[0] = 0.0f;
    m_position[1] = 0.0f;
    m_position[2] = 0.0f;
    m_color[0] = 1.0f;
    m_color[1] = 1.0f;
    m_color[2] = 1.0f;
    m_color[3] = 1.0f;
    m_dragPlaneNormal[0] = m_dragPlaneNormal[1] = m_dragPlaneNormal[2] = 0.0f;
    m_dragPlanePoint[0]  = m_dragPlanePoint[1]  = m_dragPlanePoint[2]  = 0.0f;
    m_grabOffset[0]      = m_grabOffset[1]       = m_grabOffset[2]      = 0.0f;
    m_lastCursorX = m_lastCursorY = 0.0f;
    m_angularVelocity[0] = m_angularVelocity[1] = m_angularVelocity[2] = 0.0f;
}

// ============================================================
// Shape - full constructor (uploads mesh immediately)
// ============================================================
Shape::Shape(const std::vector<float>& vertices,
             const std::vector<unsigned int>& indices,
             float r, float g, float b, float a)
    : m_vertices(vertices),
      m_indices(indices),
      m_vbo(const_cast<float*>(vertices.data()),
            static_cast<GLsizeiptr>(vertices.size() * sizeof(float))),
      m_ebo(const_cast<unsigned int*>(indices.data()),
            static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int))),
      m_vao(),
      m_indexCount(static_cast<unsigned int>(indices.size())),
      m_size(1.0f),
      m_gravityEnabled(false),
      m_cursorEnabled(true),
      m_rigidBodyEnabled(false),
      m_dragging(false),
      m_gravityController(),
      m_rotation()
{
    m_color[0] = r; m_color[1] = g; m_color[2] = b; m_color[3] = a;
    m_position[0] = m_position[1] = m_position[2] = 0.0f;
    m_dragPlaneNormal[0] = m_dragPlaneNormal[1] = m_dragPlaneNormal[2] = 0.0f;
    m_dragPlanePoint[0]  = m_dragPlanePoint[1]  = m_dragPlanePoint[2]  = 0.0f;
    m_grabOffset[0]      = m_grabOffset[1]       = m_grabOffset[2]      = 0.0f;
    m_lastCursorX = m_lastCursorY = 0.0f;
    m_angularVelocity[0] = m_angularVelocity[1] = m_angularVelocity[2] = 0.0f;

    m_vao.Bind();
    m_ebo.Bind();
    m_vao.LinkVBO(m_vbo, 0);
    m_vao.Unbind();
    m_ebo.Unbind();
}

Shape::~Shape() {
    m_vao.Delete();
    m_vbo.Delete();
    m_ebo.Delete();
}

// ============================================================
// InitializeMesh - used after default construction (e.g. Sphere)
// ============================================================
void Shape::InitializeMesh(const std::vector<float>& vertices,
                            const std::vector<unsigned int>& indices,
                            float r, float g, float b, float a) {
    m_vertices   = vertices;
    m_indices    = indices;
    m_indexCount = static_cast<unsigned int>(indices.size());
    m_color[0] = r; m_color[1] = g; m_color[2] = b; m_color[3] = a;

    // Delete old GPU resources and rebuild
    m_vao.Delete();
    m_vbo.Delete();
    m_ebo.Delete();

    m_vbo = VBO(const_cast<float*>(m_vertices.data()),
                static_cast<GLsizeiptr>(m_vertices.size() * sizeof(float)));
    m_ebo = EBO(const_cast<unsigned int*>(m_indices.data()),
                static_cast<GLsizeiptr>(m_indices.size() * sizeof(unsigned int)));
    m_vao = VAO();

    m_vao.Bind();
    m_ebo.Bind();
    m_vao.LinkVBO(m_vbo, 0);
    m_vao.Unbind();
    m_ebo.Unbind();
}

// ============================================================
// Setters
// ============================================================
void Shape::EnableGravity(bool enabled) {
    m_gravityEnabled = enabled;
    if (!enabled) m_gravityController.ResetVelocity();
}

void Shape::SetRigidBody(bool enabled) {
    m_rigidBodyEnabled = enabled;
    if (!enabled) {
        m_angularVelocity[0] = m_angularVelocity[1] = m_angularVelocity[2] = 0.0f;
        m_rotation = RotationController();
    }
}

void Shape::SetSize(float size) {
    m_size = (size < 0.1f) ? 0.1f : size;
}

void Shape::SetPosition(float x, float y, float z) {
    m_position[0] = x;
    m_position[1] = y;
    m_position[2] = z;
}

void Shape::SetColor(float r, float g, float b, float a) {
    m_color[0] = r; m_color[1] = g; m_color[2] = b; m_color[3] = a;
}

void Shape::ResetVelocity() {
    m_gravityController.ResetVelocity();
    m_angularVelocity[0] = m_angularVelocity[1] = m_angularVelocity[2] = 0.0f;
}

// ============================================================
// Update - physics step
// ============================================================
void Shape::Update(float deltaTime) {
    // Gravity
    if (m_gravityEnabled) {
        m_gravityController.UpdatePosition(m_position[1], deltaTime);
    }

    // Dampen angular velocity (simulate air/friction)
    if (m_rigidBodyEnabled) {
        const float dampFactor = std::pow(0.85f, deltaTime * 60.0f);
        m_angularVelocity[0] *= dampFactor;
        m_angularVelocity[1] *= dampFactor;
        m_angularVelocity[2] *= dampFactor;

        // Integrate rotation
        if (len3(m_angularVelocity) > 0.0001f) {
            m_rotation.AddCursorDelta(
                m_angularVelocity[1] * deltaTime * 200.0f,   // yaw from Y angular vel
                m_angularVelocity[0] * deltaTime * 200.0f    // pitch from X angular vel
            );
        }
    }
}

// ============================================================
// TestIntersect - non-destructive ray test (returns tHit or -1)
// ============================================================
float Shape::TestIntersect(const float rayOrigin[3], const float rayDir[3]) const {
    if (!m_cursorEnabled) return -1.0f;
    float t = 0.0f;
    if (!Intersect(rayOrigin, rayDir, t)) return -1.0f;
    return t;
}

// ============================================================
// Picking
// ============================================================
bool Shape::TryPick(const float rayOrigin[3], const float rayDir[3],
                    const float camFront[3], double cursorX, double cursorY) {
    if (!m_cursorEnabled) return false;

    float tHit = 0.0f;
    if (!Intersect(rayOrigin, rayDir, tHit)) return false;

    m_dragging = true;
    m_lastCursorX = static_cast<float>(cursorX);
    m_lastCursorY = static_cast<float>(cursorY);
    m_dragPlaneNormal[0] = camFront[0];
    m_dragPlaneNormal[1] = camFront[1];
    m_dragPlaneNormal[2] = camFront[2];

    // Compute the exact world-space hit point on the shape surface
    float hitPt[3] = {
        rayOrigin[0] + rayDir[0] * tHit,
        rayOrigin[1] + rayDir[1] * tHit,
        rayOrigin[2] + rayDir[2] * tHit
    };

    // Anchor the drag plane at the HIT POINT (not shape centre).
    // This ensures UpdateDrag's plane intersection gives exactly hitPt
    // on the first frame, so the shape doesn't jump.
    m_dragPlanePoint[0] = hitPt[0];
    m_dragPlanePoint[1] = hitPt[1];
    m_dragPlanePoint[2] = hitPt[2];

    // Grab offset = distance from shape centre to the surface hit point.
    m_grabOffset[0] = hitPt[0] - m_position[0];
    m_grabOffset[1] = hitPt[1] - m_position[1];
    m_grabOffset[2] = hitPt[2] - m_position[2];

    m_gravityController.ResetVelocity();
    m_angularVelocity[0] = m_angularVelocity[1] = m_angularVelocity[2] = 0.0f;
    return true;
}

void Shape::UpdateDrag(const float rayOrigin[3], const float rayDir[3],
                       double cursorX, double cursorY) {
    if (!m_dragging) return;

    float tPlane;
    if (RayPlaneIntersect(rayOrigin, rayDir, m_dragPlaneNormal, m_dragPlanePoint, tPlane)) {
        // Where the cursor ray now touches the drag plane
        float newGrabPt[3] = {
            rayOrigin[0] + rayDir[0] * tPlane,
            rayOrigin[1] + rayDir[1] * tPlane,
            rayOrigin[2] + rayDir[2] * tPlane
        };
        // Move centre so the grabbed surface point follows the cursor exactly
        m_position[0] = newGrabPt[0] - m_grabOffset[0];
        m_position[1] = newGrabPt[1] - m_grabOffset[1];
        m_position[2] = newGrabPt[2] - m_grabOffset[2];
        // Advance the drag plane to the current cursor hit point (not shape centre!)
        // This keeps the plane at the right depth as the cursor moves around.
        m_dragPlanePoint[0] = newGrabPt[0];
        m_dragPlanePoint[1] = newGrabPt[1];
        m_dragPlanePoint[2] = newGrabPt[2];
    }

    float deltaX = static_cast<float>(cursorX - m_lastCursorX);
    float deltaY = static_cast<float>(cursorY - m_lastCursorY);
    if (m_rigidBodyEnabled) {
        OnDragRotation(deltaX, deltaY,
                       m_grabOffset[0], m_grabOffset[1], m_grabOffset[2]);
    }
    m_lastCursorX = static_cast<float>(cursorX);
    m_lastCursorY = static_cast<float>(cursorY);
}

void Shape::ReleaseDrag() {
    m_dragging = false;
}

bool Shape::IsDragging() const {
    return m_dragging;
}

// ============================================================
// Rendering
// ============================================================
void Shape::Display(Shader& shader, const float viewProjection[16]) {
    float model[16];
    BuildModelMatrix(model);

    float mvp[16];
    MultiplyMat4(viewProjection, model, mvp);

    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, model);
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "MVP"),   1, GL_FALSE, mvp);
    glUniform4f(glGetUniformLocation(shader.ID, "ourColor"),
                m_color[0], m_color[1], m_color[2], m_color[3]);
    glUniform1i(glGetUniformLocation(shader.ID, "useGrid"), 0);

    m_vao.Bind();
    m_ebo.Bind();
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    m_ebo.Unbind();
    m_vao.Unbind();
}

// ============================================================
// BuildModelMatrix: Translation * Rotation * Scale
// ============================================================
void Shape::BuildModelMatrix(float out[16]) const {
    // Scale matrix
    float scale[16];
    IdentityMat4(scale);
    scale[0]  = m_size;
    scale[5]  = m_size;
    scale[10] = m_size;

    // Rotation matrix from RotationController
    float rot[16];
    m_rotation.GetRotationMatrix(rot);

    // RS = Rotation * Scale
    float rs[16];
    MultiplyMat4(rot, scale, rs);

    // Translation matrix
    float trans[16];
    TranslateMat4(trans, m_position[0], m_position[1], m_position[2]);

    // Final = Trans * RS
    MultiplyMat4(trans, rs, out);
}

// ============================================================
// Static ray helpers
// ============================================================
bool Shape::RayPlaneIntersect(const float origin[3], const float dir[3],
                               const float planeNormal[3], const float planePoint[3], float& tHit) {
    float denom = dot3(dir, planeNormal);
    if (std::fabs(denom) < 1e-6f) return false;
    float diff[3] = {planePoint[0]-origin[0], planePoint[1]-origin[1], planePoint[2]-origin[2]};
    tHit = dot3(diff, planeNormal) / denom;
    return tHit >= 0.0f;
}

bool Shape::RayIntersectsAABB(const float origin[3], const float dir[3],
                               const float mn[3], const float mx[3], float& tHit) {
    float tMin = 0.0f, tMax = 1e9f;
    for (int i = 0; i < 3; ++i) {
        if (std::fabs(dir[i]) < 1e-6f) {
            if (origin[i] < mn[i] || origin[i] > mx[i]) return false;
            continue;
        }
        float invD = 1.0f / dir[i];
        float t0 = (mn[i] - origin[i]) * invD;
        float t1 = (mx[i] - origin[i]) * invD;
        if (t0 > t1) { float tmp = t0; t0 = t1; t1 = tmp; }
        tMin = t0 > tMin ? t0 : tMin;
        tMax = t1 < tMax ? t1 : tMax;
        if (tMax < tMin) return false;
    }
    tHit = tMin;
    return true;
}

bool Shape::RayIntersectsSphere(const float origin[3], const float dir[3],
                                 const float center[3], float radius, float& tHit) {
    float oc[3] = {origin[0]-center[0], origin[1]-center[1], origin[2]-center[2]};
    float b = 2.0f * dot3(oc, dir);
    float c = dot3(oc, oc) - radius * radius;
    float disc = b*b - 4.0f*c;
    if (disc < 0.0f) return false;
    float sq = std::sqrt(disc);
    float t0 = (-b - sq) * 0.5f;
    float t1 = (-b + sq) * 0.5f;
    if (t0 > 0.0f) { tHit = t0; return true; }
    if (t1 > 0.0f) { tHit = t1; return true; }
    return false;
}

// ============================================================
// Cube
// ============================================================
Cube::Cube()
    : Shape({
          -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,0.5f,-0.5f,  -0.5f,0.5f,-0.5f,
          -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f,0.5f, 0.5f,  -0.5f,0.5f, 0.5f},
         {0,1,2,  2,3,0,
          4,5,6,  6,7,4,
          0,4,7,  7,3,0,
          1,5,6,  6,2,1,
          3,2,6,  6,7,3,
          0,1,5,  5,4,0},
         0.35f, 0.75f, 1.0f)
{}

bool Cube::Intersect(const float rayOrigin[3], const float rayDir[3], float& tHit) const {
    float half = 0.5f * m_size;
    float mn[3] = {m_position[0]-half, m_position[1]-half, m_position[2]-half};
    float mx[3] = {m_position[0]+half, m_position[1]+half, m_position[2]+half};
    return RayIntersectsAABB(rayOrigin, rayDir, mn, mx, tHit);
}

// Rigid body: torque = cross(grabOffset, dragForce)
// This makes the cube tip when grabbed off-center.
void Cube::OnDragRotation(float deltaX, float deltaY,
                           float gox, float goy, float goz) {
    // Mouse drag is a force applied at the grab offset.
    // Torque axis = grabOffset x dragForce (force is approx. screen-space delta)
    // We use yaw/pitch rotation from the RotationController for simplicity.
    const float sens = 0.004f;

    // Determine which axis the offset is on to compute tipping direction
    float offsetLen = std::sqrt(gox*gox + goy*goy + goz*goz);
    if (offsetLen < 0.01f) {
        // Grabbed near centre - uniform rotation
        m_rotation.AddCursorDelta(deltaX * 200.0f * sens, deltaY * 200.0f * sens);
        return;
    }

    // The torque from dragging: cross(grabOffset, dragVector)
    float dragForce[3] = {deltaX, -deltaY, 0.0f}; // screen-space approximation
    float grabOff[3]   = {gox, goy, goz};
    float torque[3];
    cross3(grabOff, dragForce, torque);

    // Apply torque as angular velocity impulse (rigid body feel)
    const float torqueScale = 0.08f;
    m_angularVelocity[0] += torque[0] * torqueScale;
    m_angularVelocity[1] += torque[1] * torqueScale;
    m_angularVelocity[2] += torque[2] * torqueScale;

    // Also directly rotate a bit for immediate feedback
    m_rotation.AddCursorDelta(deltaX * 100.0f * sens, deltaY * 100.0f * sens);
}

// ============================================================
// Sphere
// ============================================================
Sphere::Sphere(float radius, int sectors, int stacks)
    : Shape(), m_radius(radius)
{
    BuildSphereMesh(radius, sectors, stacks);
    // Upload mesh (was built in default-constructed Shape)
    m_color[0] = 1.0f; m_color[1] = 0.55f; m_color[2] = 0.2f; m_color[3] = 1.0f;
    InitializeMesh(m_vertices, m_indices,
                   m_color[0], m_color[1], m_color[2], m_color[3]);
}

void Sphere::BuildSphereMesh(float radius, int sectors, int stacks) {
    const float PI = 3.14159265358979323846f;
    m_vertices.clear();
    m_indices.clear();
    for (int stack = 0; stack <= stacks; ++stack) {
        float phi   = PI/2.0f - stack * PI / stacks;
        float y     = radius * std::sin(phi);
        float scale = radius * std::cos(phi);
        for (int sector = 0; sector <= sectors; ++sector) {
            float theta = sector * 2.0f * PI / sectors;
            m_vertices.push_back(scale * std::cos(theta));
            m_vertices.push_back(y);
            m_vertices.push_back(scale * std::sin(theta));
        }
    }
    for (int stack = 0; stack < stacks; ++stack) {
        int ss = stack * (sectors+1);
        int ns = (stack+1) * (sectors+1);
        for (int sector = 0; sector < sectors; ++sector) {
            m_indices.push_back(ss + sector);
            m_indices.push_back(ns + sector);
            m_indices.push_back(ns + sector + 1);
            m_indices.push_back(ss + sector);
            m_indices.push_back(ns + sector + 1);
            m_indices.push_back(ss + sector + 1);
        }
    }
}

bool Sphere::Intersect(const float rayOrigin[3], const float rayDir[3], float& tHit) const {
    float center[3] = {m_position[0], m_position[1], m_position[2]};
    return RayIntersectsSphere(rayOrigin, rayDir, center, m_radius * m_size, tHit);
}

void Sphere::OnDragRotation(float deltaX, float deltaY,
                             float gox, float goy, float goz) {
    const float sens = 0.004f;
    float dragForce[3] = {deltaX, -deltaY, 0.0f};
    float grabOff[3]   = {gox, goy, goz};
    float torque[3];
    cross3(grabOff, dragForce, torque);
    const float scale = 0.06f;
    m_angularVelocity[0] += torque[0] * scale;
    m_angularVelocity[1] += torque[1] * scale;
    m_rotation.AddCursorDelta(deltaX * 80.0f * sens, deltaY * 80.0f * sens);
}

// ============================================================
// Cylinder
// ============================================================
Cylinder::Cylinder(float radius, float height, int sectors)
    : Shape(), m_radius(radius), m_height(height)
{
    BuildCylinderMesh(radius, height, sectors);
    m_color[0] = 0.6f; m_color[1] = 0.3f; m_color[2] = 0.9f; m_color[3] = 1.0f;
    InitializeMesh(m_vertices, m_indices,
                   m_color[0], m_color[1], m_color[2], m_color[3]);
    // Cylinder centre rests at half-height above the floor
    m_gravityController.SetFloorY(m_height * 0.5f);
}

void Cylinder::BuildCylinderMesh(float radius, float height, int sectors) {
    const float PI = 3.14159265358979323846f;
    m_vertices.clear();
    m_indices.clear();

    float halfH = height * 0.5f;

    // Side vertices: bottom ring then top ring
    for (int i = 0; i <= sectors; ++i) {
        float theta = i * 2.0f * PI / sectors;
        float x = radius * std::cos(theta);
        float z = radius * std::sin(theta);
        m_vertices.push_back(x); m_vertices.push_back(-halfH); m_vertices.push_back(z);
        m_vertices.push_back(x); m_vertices.push_back( halfH); m_vertices.push_back(z);
    }

    // Side faces
    for (int i = 0; i < sectors; ++i) {
        unsigned int b0 = i * 2;
        unsigned int t0 = b0 + 1;
        unsigned int b1 = (i+1) * 2;
        unsigned int t1 = b1 + 1;
        m_indices.push_back(b0); m_indices.push_back(b1); m_indices.push_back(t0);
        m_indices.push_back(t0); m_indices.push_back(b1); m_indices.push_back(t1);
    }

    // Cap centres
    unsigned int botCentre = static_cast<unsigned int>(m_vertices.size() / 3);
    m_vertices.push_back(0.0f); m_vertices.push_back(-halfH); m_vertices.push_back(0.0f);
    unsigned int topCentre = static_cast<unsigned int>(m_vertices.size() / 3);
    m_vertices.push_back(0.0f); m_vertices.push_back( halfH); m_vertices.push_back(0.0f);

    for (int i = 0; i < sectors; ++i) {
        unsigned int b0 = i * 2;
        unsigned int b1 = (i+1) * 2;
        unsigned int t0 = b0 + 1;
        unsigned int t1 = b1 + 1;
        // Bottom cap (wound inward)
        m_indices.push_back(botCentre); m_indices.push_back(b1); m_indices.push_back(b0);
        // Top cap
        m_indices.push_back(topCentre); m_indices.push_back(t0); m_indices.push_back(t1);
    }
}

bool Cylinder::Intersect(const float rayOrigin[3], const float rayDir[3], float& tHit) const {
    // AABB approximation for picking (cylinder fits inside this box)
    float half  = m_radius * m_size;
    float halfH = m_height * m_size * 0.5f;
    float mn[3] = {m_position[0]-half, m_position[1]-halfH, m_position[2]-half};
    float mx[3] = {m_position[0]+half, m_position[1]+halfH, m_position[2]+half};
    return RayIntersectsAABB(rayOrigin, rayDir, mn, mx, tHit);
}

void Cylinder::OnDragRotation(float deltaX, float deltaY,
                               float gox, float goy, float goz) {
    const float sens = 0.004f;
    float dragForce[3] = {deltaX, -deltaY, 0.0f};
    float grabOff[3]   = {gox, goy, goz};
    float torque[3];
    cross3(grabOff, dragForce, torque);
    const float scale = 0.07f;
    m_angularVelocity[0] += torque[0] * scale;
    m_angularVelocity[1] += torque[1] * scale;
    m_rotation.AddCursorDelta(deltaX * 100.0f * sens, deltaY * 100.0f * sens);
}

// ============================================================
// Capsule
// ============================================================
Capsule::Capsule(float radius, float height, int sectors, int stacks)
    : Shape(), m_radius(radius), m_height(height)
{
    BuildCapsuleMesh(radius, height, sectors, stacks);
    m_color[0] = 1.0f; m_color[1] = 0.75f; m_color[2] = 0.3f; m_color[3] = 1.0f;
    InitializeMesh(m_vertices, m_indices,
                   m_color[0], m_color[1], m_color[2], m_color[3]);
    // Capsule centre rests at (halfHeight + radius) above the floor
    m_gravityController.SetFloorY(m_height * 0.5f + m_radius);
}

void Capsule::BuildCapsuleMesh(float radius, float height, int sectors, int stacks) {
    const float PI = 3.14159265358979323846f;
    m_vertices.clear();
    m_indices.clear();

    float halfH = height * 0.5f;
    int vertCount = 0;

    // Top hemisphere
    for (int stack = 0; stack <= stacks; ++stack) {
        float phi = (PI/2.0f) * (static_cast<float>(stack) / stacks);
        float y   = radius * std::sin(phi) + halfH;
        float r   = radius * std::cos(phi);
        for (int sector = 0; sector <= sectors; ++sector) {
            float theta = sector * 2.0f * PI / sectors;
            m_vertices.push_back(r * std::cos(theta));
            m_vertices.push_back(y);
            m_vertices.push_back(r * std::sin(theta));
        }
    }

    // Cylinder body (just bottom ring of top hem already has top edge,
    // we add one more ring at -halfH and connect)
    for (int sector = 0; sector <= sectors; ++sector) {
        float theta = sector * 2.0f * PI / sectors;
        m_vertices.push_back(radius * std::cos(theta));
        m_vertices.push_back(-halfH);
        m_vertices.push_back(radius * std::sin(theta));
    }

    // Bottom hemisphere
    for (int stack = 0; stack <= stacks; ++stack) {
        float phi = (PI/2.0f) * (static_cast<float>(stack) / stacks);
        float y   = -(radius * std::sin(phi) + halfH);
        float r   = radius * std::cos(phi);
        for (int sector = 0; sector <= sectors; ++sector) {
            float theta = sector * 2.0f * PI / sectors;
            m_vertices.push_back(r * std::cos(theta));
            m_vertices.push_back(y);
            m_vertices.push_back(r * std::sin(theta));
        }
    }

    int totalRings = (stacks + 1) + 1 + (stacks + 1); // top hem + cylinder + bottom hem
    // Build triangles between consecutive rings
    for (int ring = 0; ring < totalRings - 1; ++ring) {
        for (int sector = 0; sector < sectors; ++sector) {
            unsigned int a = ring * (sectors+1) + sector;
            unsigned int b = a + (sectors+1);
            unsigned int c = a + 1;
            unsigned int d = b + 1;
            m_indices.push_back(a); m_indices.push_back(b); m_indices.push_back(c);
            m_indices.push_back(c); m_indices.push_back(b); m_indices.push_back(d);
        }
    }
}

bool Capsule::Intersect(const float rayOrigin[3], const float rayDir[3], float& tHit) const {
    float totalHalf = (m_height * 0.5f + m_radius) * m_size;
    float halfR     = m_radius * m_size;
    float mn[3] = {m_position[0]-halfR,      m_position[1]-totalHalf, m_position[2]-halfR};
    float mx[3] = {m_position[0]+halfR,      m_position[1]+totalHalf, m_position[2]+halfR};
    return RayIntersectsAABB(rayOrigin, rayDir, mn, mx, tHit);
}

void Capsule::OnDragRotation(float deltaX, float deltaY,
                              float gox, float goy, float goz) {
    const float sens = 0.004f;
    float dragForce[3] = {deltaX, -deltaY, 0.0f};
    float grabOff[3]   = {gox, goy, goz};
    float torque[3];
    cross3(grabOff, dragForce, torque);
    const float scale = 0.07f;
    m_angularVelocity[0] += torque[0] * scale;
    m_angularVelocity[1] += torque[1] * scale;
    m_rotation.AddCursorDelta(deltaX * 100.0f * sens, deltaY * 100.0f * sens);
}
