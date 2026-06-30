# Physics Engine from Scratch

## My Journey 🚀

I discovered a passion for low-level, performance-critical code while scrolling through YouTube. Watching countless creators build physics engines from scratch sparked something in me. I had recently learned C++, and thought to myself: **"Why not make myself suffer and build one too?"**

And here we are.

This repository documents my adventure into graphics programming and physics simulation. What started as curiosity has become an exercise in understanding how the digital world actually works beneath the layers of abstraction we usually rely on.

---

## About This Project

This is a **from-scratch physics engine** built with:
- **C++17** for performance and low-level control
- **OpenGL 3.3 Core Profile** for graphics rendering
- **GLFW** for window management and inputs
- **GLAD** for OpenGL function loading
- **Dear ImGui** for developer UI and scene control

---

## Core Features 💡

- **Polymorphic Shapes**: Fully implemented procedural geometry generation for:
  - **Cubes**
  - **Spheres** (spherical coordinate sector/stack generation)
  - **Cylinders** (parametric base rings, side panels, and capped ends)
  - **Capsules** (hemisphere caps connected by a central cylindrical body)
- **Physics Simulation**:
  - **Gravity**: Per-object configurable gravity with ground collision detection matching each shape's custom Y-height extents.
  - **Rigid Body Rotation**: Torque integration based on cursor drag offsets. Objects tilt and roll realistically when pulled from off-center points.
- **Interactive UI Editor (Dear ImGui)**:
  - **Spawn Panel**: Instantly spawn any shape in front of the camera with global gravity controls.
  - **Properties Panel**: Modify scale, physics parameters (gravity on/off, rigid body toggle), colors, and reset velocity or delete individual objects.
  - **Status bar**: Real-time status display and mode indicators.
- **3D Ray Picking & Dragging**: Fully implemented perspective ray-cast picking to select and manipulate shapes in 3D space with depth-based drag plane updates.
- **First-Person Camera**: Fly-through camera controls using WASD + mouse look (toggle using **TAB**).

---

## Project Structure

```
physics_engine/
├── src/
│   ├── main.cpp                # Entry point, UI render loop, and picking handlers
│   ├── Shapes.cpp              # Mesh generation and physics updates
│   ├── Movement.cpp            # Camera control and callback handling
│   ├── GravityController.cpp   # Vertical fall and floor alignment logic
│   ├── RotationController.cpp  # Yaw/pitch orientation helper
│   ├── glad.c                  # OpenGL function loader
│   ├── shaderClass.cpp         # Shader loading and compiling
│   ├── VAO.cpp / VBO.cpp / EBO.cpp # GPU buffer wrappers
│   └── imgui*.cpp              # ImGui library implementation
├── include/                    # Headers
│   ├── Shapes.h
│   ├── Movement.h
│   ├── GravityController.h
│   ├── RotationController.h
│   ├── MathUtils.h             # Lightweight 3D matrix math library
│   ├── shaderClass.h
│   ├── VAO.h / VBO.h / EBO.h
│   └── imgui*.h
├── Resources/
│   └── Shaders/                # GLSL Vertex & Fragment Shaders
└── README.md                   # This file
```

---

## Why This Matters

In a world of high-level frameworks and engines, building from scratch teaches you:
- **How graphics actually work** — Not just calling pre-built engine functions.
- **Performance consciousness** — Understanding trade-offs and layout optimization.
- **Debugging skills** — Figuring out what went wrong at the lowest levels.
- **Pride in creation** — You understand every line of code.

---

## Special Thanks

- **YouTube creators** whose tutorials provided guidance and inspiration.
- **Documentation and wiki resources** that answered my questions.
- **The C++ community** for maintaining excellent low-level resources.

---

**Status:** 🔨 **In Development** — This engine is actively being built and refined.

---

*"The best way to learn is to do. And sometimes, doing means making yourself suffer through a physics engine from scratch."*
