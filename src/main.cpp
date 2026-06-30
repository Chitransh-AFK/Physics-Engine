// ============================================================
// PhysicsEngine - main.cpp
//
// Entry point for the 3D physics sandbox.
// Uses OpenGL 3.3 Core Profile for rendering and Dear ImGui
// (immediate-mode GUI) for the editor panels.
//
// Architecture overview:
//   - MovementController  : first-person camera (WASD + mouse)
//   - Shape hierarchy     : Cube / Sphere / Cylinder / Capsule
//   - GravityController   : per-shape free-fall + floor collision
//   - RotationController  : yaw/pitch drag rotation
//   - Dear ImGui panels   : Spawn, Properties, Status bar
//
// Controls:
//   TAB         - toggle cursor  (free-cursor = UI + picking,
//                                 captured    = camera look)
//   W/A/S/D     - fly camera (captured mode)
//   Space/Shift - move camera up / down
//   Left-click  - pick and drag a shape (free-cursor mode)
// ============================================================

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>
#include <memory>      // unique_ptr
#include <string>
#include <algorithm>   // remove_if

// Dear ImGui - immediate-mode GUI library.
// The GLFW + OpenGL3 backends handle platform integration.
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "shaderClass.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include "Movement.h"    // MovementController (camera)
#include "MathUtils.h"   // Radians, Perspective, MultiplyMat4, etc.
#include "Shapes.h"      // Shape base + Cube / Sphere / Cylinder / Capsule

// ============================================================
// Ray utility functions used for 3D picking
// ============================================================

// Compute the cross product  out = a × b.
static void cross3(const float a[3], const float b[3], float out[3]) {
    out[0] = a[1]*b[2] - a[2]*b[1];
    out[1] = a[2]*b[0] - a[0]*b[2];
    out[2] = a[0]*b[1] - a[1]*b[0];
}

// Normalize a 3-component vector in place (safe: no-op if zero-length).
static void normalize3(float v[3]) {
    float len = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (len > 0.0f) { v[0] /= len; v[1] /= len; v[2] /= len; }
}

// Build a world-space ray from the camera through a screen pixel.
//
// The ray is computed in camera-space using the perspective projection
// parameters (aspect ratio, field of view), then converted to world
// space via the camera's right and up vectors.
//
//   camPos/Front/Up  : camera transform from MovementController
//   aspect           : viewport width / height
//   fovRad           : vertical field-of-view in radians
//   cx, cy           : cursor position in screen pixels (GLFW coords)
//   w, h             : framebuffer dimensions in pixels
//   rayOrigin/Dir    : output world-space ray (origin + normalised direction)
static void rayFromCursor(const float camPos[3], const float camFront[3], const float camUp[3],
                           float aspect, float fovRad,
                           double cx, double cy, int w, int h,
                           float rayOrigin[3], float rayDir[3]) {
    // Convert cursor to NDC then scale by aspect so x spans the full fov.
    float x = (2.0f * (float)cx / w - 1.0f) * aspect;
    float y =  1.0f - 2.0f * (float)cy / h;    // invert Y (screen is top-down)
    float tanFov = std::tan(fovRad / 2.0f);

    // Camera right vector (perpendicular to front and up).
    float right[3];
    cross3(camFront, camUp, right);
    normalize3(right);

    float up[3] = {camUp[0], camUp[1], camUp[2]};
    normalize3(up);

    // Ray origin = camera position.
    rayOrigin[0] = camPos[0];
    rayOrigin[1] = camPos[1];
    rayOrigin[2] = camPos[2];

    // Ray direction = forward + horizontal offset + vertical offset.
    rayDir[0] = camFront[0] + right[0]*x*tanFov + up[0]*y*tanFov;
    rayDir[1] = camFront[1] + right[1]*x*tanFov + up[1]*y*tanFov;
    rayDir[2] = camFront[2] + right[2]*x*tanFov + up[2]*y*tanFov;
    normalize3(rayDir);
}

// Called by GLFW whenever the framebuffer is resized.
// Keeps the OpenGL viewport in sync with the window dimensions.
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// ============================================================
// main()
// ============================================================
int main() {

    // ---- Initialise GLFW ----
    if (!glfwInit()) { std::cerr << "GLFW init failed\n"; return -1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const int WIN_W = 1280, WIN_H = 720;
    GLFWwindow* window = glfwCreateWindow(WIN_W, WIN_H, "Physics Engine", NULL, NULL);
    if (!window) { std::cerr << "Window creation failed\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);   // v-sync: cap at monitor refresh rate

    // ---- Initialise GLAD (load OpenGL function pointers) ----
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD init failed\n"; glfwDestroyWindow(window); glfwTerminate(); return -1;
    }
    glViewport(0, 0, WIN_W, WIN_H);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ---- Create the camera / movement controller ----
    // IMPORTANT: MovementController MUST be created BEFORE calling
    // ImGui_ImplGlfw_InitForOpenGL.
    //
    // ImGui's GLFW backend installs its own GLFW callbacks with
    // install_callbacks=true, which chains them ONTO whichever callbacks
    // are already registered.  If ImGui were initialised first, the
    // movement controller's glfwSetCursorPosCallback / glfwSetKeyCallback
    // calls would silently overwrite ImGui's hooks and break all UI input.
    MovementController movement(window);

    // ---- Initialise Dear ImGui ----
    // install_callbacks=true  ->  ImGui's callbacks chain after Movement's.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Use the built-in "Classic" theme - plain grey developer-tool look.
    ImGui::StyleColorsClassic();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;   // square windows
    style.FrameRounding  = 0.0f;
    style.GrabRounding   = 0.0f;
    style.WindowPadding  = ImVec2(8, 8);
    style.ItemSpacing    = ImVec2(6, 4);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // ---- Load GLSL shaders from disk ----
    Shader shader("../Resources/Shaders/default.vert", "../Resources/Shaders/default.frag");

    // ---- Ground plane geometry ----
    // A large flat quad at Y = 0.  The fragment shader renders a grid
    // pattern on it when the "useGrid" uniform is set to 1.
    float planeVerts[] = {
        -20.0f, 0.0f, -20.0f,
         20.0f, 0.0f, -20.0f,
         20.0f, 0.0f,  20.0f,
        -20.0f, 0.0f,  20.0f
    };
    unsigned int planeIdx[] = {0, 1, 2, 2, 3, 0};
    VAO planeVao;
    VBO planeVbo(planeVerts, sizeof(planeVerts));
    EBO planeEbo((GLuint*)planeIdx, sizeof(planeIdx));
    planeVao.Bind();
    planeEbo.Bind();
    planeVao.LinkVBO(planeVbo, 0);
    planeVao.Unbind();
    planeEbo.Unbind();

    glEnable(GL_DEPTH_TEST);   // enable depth buffering for correct occlusion

    // ---- Scene / editor state ----
    std::vector<std::unique_ptr<Shape>> shapes;   // all live shapes in the scene
    Shape* selectedShape = nullptr;               // currently highlighted / dragged shape
    bool   globalGravity = true;                  // default: gravity on for new shapes
    int    spawnCount    = 0;                     // used to spread shapes on spawn
    bool   prevLeft      = false;                 // left-button state from previous frame

    float lastFrame = (float)glfwGetTime();

    // ============================================================
    // Main render loop
    // ============================================================
    while (!glfwWindowShouldClose(window)) {

        // --- Frame timing ---
        float now       = (float)glfwGetTime();
        float deltaTime = now - lastFrame;
        if (deltaTime > 0.05f) deltaTime = 0.05f;  // clamp: prevents huge jumps on pause
        lastFrame = now;

        glfwPollEvents();

        // Framebuffer size (may differ from window size on HiDPI displays).
        int fbW, fbH;
        glfwGetFramebufferSize(window, &fbW, &fbH);
        float aspect = (fbH == 0) ? (16.0f/9.0f) : (float)fbW / (float)fbH;

        // ---- Begin ImGui frame ----
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ========================================================
        // PANEL 1: Spawn panel (top-left)
        //   Buttons to add shapes to the scene.
        //   Global gravity checkbox applies to all existing + future shapes.
        // ========================================================
        ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(155, 240), ImGuiCond_Always);
        ImGui::Begin("Spawn", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        ImGui::Text("Spawn Shape:");
        ImGui::Separator();

        // Helper lambda: place a new shape 4 units in front of the camera,
        // with slight X spread so multiple spawns don't perfectly overlap.
        auto spawnAt = [&](std::unique_ptr<Shape> s) {
            const float* cp = movement.GetCameraPosition();
            const float* cf = movement.GetCameraFront();
            float spread = (spawnCount % 3 - 1) * 0.9f;   // -0.9, 0, +0.9 cycling
            s->SetPosition(cp[0] + cf[0]*4.0f + spread,
                           cp[1] + cf[1]*4.0f + 1.5f,
                           cp[2] + cf[2]*4.0f);
            s->EnableGravity(globalGravity);
            shapes.push_back(std::move(s));
            ++spawnCount;
        };

        // Each button creates a default-sized shape in front of the camera.
        if (ImGui::Button("Cube",     ImVec2(138, 22))) spawnAt(std::make_unique<Cube>());
        if (ImGui::Button("Sphere",   ImVec2(138, 22))) spawnAt(std::make_unique<Sphere>(0.5f, 28, 18));
        if (ImGui::Button("Capsule",  ImVec2(138, 22))) spawnAt(std::make_unique<Capsule>(0.3f, 1.0f, 24, 8));
        if (ImGui::Button("Cylinder", ImVec2(138, 22))) spawnAt(std::make_unique<Cylinder>(0.4f, 1.2f, 24));

        ImGui::Separator();

        // Global gravity toggle: when changed, propagate to every shape already
        // in the scene so they all respond immediately.
        ImGui::Checkbox("Global Gravity", &globalGravity);
        if (ImGui::IsItemEdited()) {
            for (auto& s : shapes) s->EnableGravity(globalGravity);
        }

        ImGui::Separator();

        // Remove all shapes from the scene and clear selection.
        if (ImGui::Button("Clear All", ImVec2(138, 22))) {
            selectedShape = nullptr;
            shapes.clear();
        }

        ImGui::End();

        // ========================================================
        // PANEL 2: Properties panel (top-right)
        //   Only shown when a shape is selected.
        //   Lets the user tweak the selected shape's physics and appearance.
        // ========================================================
        if (selectedShape != nullptr) {
            ImGui::SetNextWindowPos(ImVec2((float)fbW - 210, 5), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(205, 295), ImGuiCond_Always);
            ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

            // Shape type name (Cube / Sphere / Cylinder / Capsule)
            ImGui::Text("Shape: %s", selectedShape->GetShapeName().c_str());
            ImGui::Separator();

            // Rigid body toggle: when ON, dragging off-centre applies realistic torque.
            bool rb = selectedShape->IsRigidBody();
            if (ImGui::Checkbox("Rigid Body", &rb))    selectedShape->SetRigidBody(rb);

            // Per-shape gravity override (independent of the global flag).
            bool grav = selectedShape->IsGravityEnabled();
            if (ImGui::Checkbox("Gravity", &grav))     selectedShape->EnableGravity(grav);

            ImGui::Separator();

            // Uniform scale slider.
            float sz = selectedShape->GetSize();
            ImGui::Text("Size: %.2f", sz);
            if (ImGui::SliderFloat("##size", &sz, 0.2f, 4.0f)) selectedShape->SetSize(sz);

            ImGui::Separator();

            // Colour picker.
            // We copy the colour values into a local array first because
            // GetColor() returns a pointer directly into the shape's m_color
            // buffer.  If we then called SetColor() while holding that pointer,
            // the buffer would be mutated and the pointer would dangle.
            const float* col = selectedShape->GetColor();
            float c[4] = {col[0], col[1], col[2], col[3]};   // safe local copy
            ImGui::Text("Color:");
            if (ImGui::ColorEdit3("##col", c)) selectedShape->SetColor(c[0], c[1], c[2], c[3]);

            ImGui::Separator();

            // Live position readout (read-only).
            const float* pos = selectedShape->GetPosition();
            ImGui::Text("Pos: %.1f  %.1f  %.1f", pos[0], pos[1], pos[2]);

            ImGui::Separator();

            // Reset linear + angular velocity to zero (stops bouncing / spinning).
            if (ImGui::Button("Reset Vel",    ImVec2(92, 22))) selectedShape->ResetVelocity();
            ImGui::SameLine();
            // Deselect without deleting.
            if (ImGui::Button("Deselect",     ImVec2(92, 22))) selectedShape = nullptr;

            // Erase this shape from the scene.
            if (ImGui::Button("Delete Shape", ImVec2(-1,  22))) {
                shapes.erase(std::remove_if(shapes.begin(), shapes.end(),
                    [&](const std::unique_ptr<Shape>& s){ return s.get() == selectedShape; }),
                    shapes.end());
                selectedShape = nullptr;
            }

            ImGui::End();
        }

        // ========================================================
        // PANEL 3: Status bar (bottom-right, no title bar)
        //   Shows current interaction mode and scene shape count.
        // ========================================================
        {
            ImGui::SetNextWindowPos(ImVec2((float)fbW - 210, (float)fbH - 50), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(205, 45), ImGuiCond_Always);
            ImGui::Begin("##hints", nullptr,
                         ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
            if (movement.IsCursorDetached())
                ImGui::Text("Mode: UI  (TAB = camera)");
            else
                ImGui::Text("Mode: Camera  (TAB = UI)");
            ImGui::Text("Shapes: %d", (int)shapes.size());
            ImGui::End();
        }

        // ========================================================
        // Physics update (runs every frame regardless of cursor mode)
        // ========================================================
        for (auto& s : shapes) s->Update(deltaTime);

        // ========================================================
        // 3D Picking and dragging
        //
        // Only active when the cursor is in free mode (TAB pressed).
        // ImGui gets first priority: if it wants the mouse (e.g. hovering
        // over a panel) we skip 3D interaction entirely.
        // ========================================================
        double curX, curY;
        glfwGetCursorPos(window, &curX, &curY);

        // Detect leading edge of left button press.
        bool leftNow  = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
        bool justDown = leftNow && !prevLeft;   // true only on the first frame of a click
        prevLeft      = leftNow;

        bool cursorFree     = movement.IsCursorDetached();
        bool imguiWantMouse = io.WantCaptureMouse;

        if (cursorFree && !imguiWantMouse) {
            // Build a ray from the camera through the current cursor position.
            const float* cp = movement.GetCameraPosition();
            const float* cf = movement.GetCameraFront();
            const float* cu = movement.GetCameraUp();
            float ro[3], rd[3];
            rayFromCursor(cp, cf, cu, aspect, Radians(45.0f),
                          curX, curY, fbW, fbH, ro, rd);

            // --- Fresh click: find the closest shape hit by the ray ---
            // We use TestIntersect (non-destructive, returns tHit or -1)
            // on every shape to determine which one is nearest to the camera.
            // Only then do we call TryPick (which activates dragging) on the
            // winner.  This ensures a shape closer to the camera is always
            // preferred over one that happens to be first in the vector.
            if (justDown) {
                // Release any drag that may have become stale.
                for (auto& s : shapes) if (s->IsDragging()) s->ReleaseDrag();
                selectedShape = nullptr;

                float  bestT     = 1e9f;
                Shape* bestShape = nullptr;
                for (auto& s : shapes) {
                    float t = s->TestIntersect(ro, rd);
                    if (t > 0.0f && t < bestT) {
                        bestT     = t;
                        bestShape = s.get();
                    }
                }
                // Activate picking (sets up drag plane, grab offset, etc.)
                if (bestShape && bestShape->TryPick(ro, rd, cf, curX, curY)) {
                    selectedShape = bestShape;
                }
            }

            // --- Drag in progress: update position every frame ---
            if (selectedShape && selectedShape->IsDragging() && leftNow)
                selectedShape->UpdateDrag(ro, rd, curX, curY);

            // --- Button released: end the drag ---
            if (selectedShape && selectedShape->IsDragging() && !leftNow)
                selectedShape->ReleaseDrag();

        } else {
            // Cursor is captured for camera look: release any active drag.
            if (selectedShape && selectedShape->IsDragging())
                selectedShape->ReleaseDrag();
        }

        // Update camera (processes keyboard input and recomputes view matrix).
        movement.Update();

        // ========================================================
        // Render
        // ========================================================
        glClearColor(0.12f, 0.14f, 0.16f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Activate();

        // Build projection and view-projection matrices.
        float proj[16];
        Perspective(Radians(45.0f), aspect, 0.1f, 200.0f, proj);

        float vp[16];
        MultiplyMat4(proj, movement.GetViewMatrix(), vp);

        // --- Ground plane ---
        float planeModel[16]; IdentityMat4(planeModel);
        float planeMVP[16];   MultiplyMat4(vp, planeModel, planeMVP);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, planeModel);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "MVP"),   1, GL_FALSE, planeMVP);
        glUniform1i(glGetUniformLocation(shader.ID, "useGrid"), 1);   // enable grid shader
        planeVao.Bind(); planeEbo.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        planeEbo.Unbind(); planeVao.Unbind();

        // --- Shapes ---
        for (auto& s : shapes) {
            if (s.get() == selectedShape) {
                // Highlight the selected shape by brightening it temporarily.
                //
                // IMPORTANT: GetColor() returns a pointer directly into
                // the shape's internal m_color array.  We must copy the values
                // to local floats BEFORE calling SetColor(), because SetColor()
                // mutates m_color - after which the pointer would read the
                // modified (brightened) values instead of the originals,
                // causing the highlight colour to permanently replace the
                // original colour after one frame.
                const float* col = s->GetColor();
                float r = col[0], g = col[1], b = col[2], a = col[3]; // local copy
                s->SetColor(std::min(r + 0.3f, 1.0f),
                            std::min(g + 0.3f, 1.0f),
                            std::min(b + 0.3f, 1.0f), a);
                s->Display(shader, vp);
                s->SetColor(r, g, b, a);   // restore from local copy (not from pointer)
            } else {
                s->Display(shader, vp);
            }
        }

        // Render ImGui draw lists on top of the 3D scene.
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // ---- Cleanup ----
    shapes.clear();                  // calls Shape destructors -> frees GPU buffers
    planeVao.Delete();
    planeVbo.Delete();
    planeEbo.Delete();
    shader.Delete();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
