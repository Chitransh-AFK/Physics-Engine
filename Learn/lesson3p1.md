# Lesson 3 (Part 1): Hello Window — Rendering a Color & the Graphics Pipeline

> Original study notes for the *Physics_Engine* learning journal.
> Reference: https://learnopengl.com/Getting-started/Hello-Window (not reproduced — concepts explained independently)
> Continues from lesson2.md. Covers everything up to (but not including) Vertex Buffer Objects — see lesson3p2.md for that.

## Clearing the screen to a color

Right now the window from lesson 2 just shows whatever garbage was left in the framebuffer. To render something deliberate, add this inside the render loop, before swapping buffers:

```cpp
glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // state-setting: stores the color
glClear(GL_COLOR_BUFFER_BIT);         // state-using: applies it
```

This is a clean example of the state-machine pattern from lesson 1: `glClearColor` just updates a value in the context; `glClear` is the function that actually *acts* on that stored value.

`GL_COLOR_BUFFER_BIT` tells `glClear` which buffer to clear — OpenGL also has depth and stencil buffers that can be cleared independently (combined with `|` when you need more than one, e.g. `GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT`).

## Updated render loop

```cpp
while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
}
```

Run this and the window should now show a solid teal-gray background instead of garbage data — confirmation that the render loop and context are both working correctly.

## Why drawing a shape is a bigger leap

Filling the background is a single function call. Drawing an actual triangle or mesh is not — it requires understanding the **graphics pipeline**, the sequence of stages the GPU runs your vertex data through before pixels land on screen.

## The graphics pipeline, stage by stage

```
Vertex Data → Vertex Shader → Primitive Assembly → Geometry Shader (optional)
            → Rasterization → Fragment Shader → Tests & Blending → Framebuffer
```

- **Vertex Data** — raw coordinates (and optionally color, normals, texture coords) you provide, e.g. 3 floats per vertex for a triangle.
- **Vertex Shader** — a small program you write that runs *once per vertex*. Its main job is transforming vertex positions into a different coordinate space (later lessons: model → world → view → clip space).
- **Primitive Assembly** — groups vertices into the requested primitive shape (triangle, line, point) based on how you call the draw function.
- **Geometry Shader (optional)** — can generate new vertices/primitives from existing ones. Not used in this project yet.
- **Rasterization** — converts each primitive into the actual pixels (fragments) it covers on screen.
- **Fragment Shader** — runs *once per fragment* (roughly: once per pixel covered), calculating its final color.
- **Tests & Blending** — depth testing, stencil testing, and alpha blending decide whether/how each fragment's color actually gets written to the framebuffer.

The takeaway: in modern OpenGL, *you* write the vertex shader and fragment shader yourself — nothing is automatic.

## Normalized Device Coordinates (NDC)

Before any vertex shader logic, it helps to know what coordinate range OpenGL expects to actually render: any vertex outside the **-1 to 1** range on all three axes (x, y, z) is clipped and won't be visible. This cube is called Normalized Device Coordinates.

A simple triangle in NDC, defined directly (no transformation needed yet):

```cpp
float vertices[] = {
    -0.5f, -0.5f, 0.0f,  // bottom-left
     0.5f, -0.5f, 0.0f,  // bottom-right
     0.0f,  0.5f, 0.0f   // top
};
```

This is just a plain C array sitting in CPU (RAM) memory right now — it knows nothing about the GPU. Getting it onto the GPU, where the vertex shader can actually read it, is the job of a **Vertex Buffer Object**, covered next in lesson3p2.md.
