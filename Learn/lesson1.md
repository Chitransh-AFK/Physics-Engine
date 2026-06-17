# Lesson 1: What is OpenGL?

> Original study notes for the *Physics_Engine* learning journal.
> Reference: https://learnopengl.com/Getting-started/OpenGL (not reproduced — concepts explained independently)

## OpenGL is a specification, not a library

OpenGL is best thought of as a **specification** — a document describing what each function should do and what results it should produce. It doesn't ship as a single library you download; instead, GPU vendors (NVIDIA, AMD, Intel) write **drivers** that implement this specification in hardware-accelerated code. When you call an OpenGL function, you're really calling into the vendor's driver implementation.

This is why behavior can sometimes differ slightly between vendors — the spec defines the contract, but implementation details are up to the driver author.

## Core-profile vs. legacy (immediate mode)

OpenGL has evolved over decades:

- **Immediate mode (legacy, fixed-function pipeline)** — easy to use, but hides almost everything from the programmer (matrix math, lighting, etc. handled internally). Deprecated since OpenGL 3.2.
- **Core-profile (modern OpenGL)** — strips away the convenience functions and forces you to understand exactly how the GPU pipeline works: shaders, buffers, and vertex layouts are all explicit. More code up front, but far more flexibility and performance.

Modern engines (and this project) target core-profile OpenGL.

## OpenGL as a state machine

A useful mental model: OpenGL is a giant **state machine**. The *context* holds a large collection of variables describing how OpenGL should currently behave — the active shader, the bound buffer, the clear color, blending mode, and so on.

Two categories of functions operate on this state:

1. **State-setting functions** — change a variable in the context (e.g. setting the current clear color).
2. **State-using functions** — execute using whatever the current state happens to be (e.g. the actual clear call uses whatever color was last set).

Because of this, the *order* in which you call OpenGL functions matters enormously — binding the wrong object before a draw call will silently use the wrong data.

## OpenGL objects

Most things you create in OpenGL — buffers, textures, shader programs — are represented as **objects**. An object here just means a struct of state stored on the GPU side, referenced from your code by an integer ID.

The typical lifecycle pattern looks like this:

```c
unsigned int objectId = 0;
glGenObject(1, &objectId);     // create / reserve an ID
glBindObject(GL_TARGET, objectId); // make it "current" in the state machine
glSetObjectOption(GL_TARGET, GL_OPTION, value); // configure it
glBindObject(GL_TARGET, 0);    // unbind when done
glDeleteObject(1, &objectId);  // free GPU memory
```

You'll see this exact bind → configure → use → unbind rhythm repeat for buffers, textures, and framebuffers throughout the engine.

## Why we need helper libraries

Two practical problems stand in the way of using raw OpenGL:

- **Window & context creation isn't part of OpenGL itself** — it's handled by the OS. **GLFW** abstracts this across Windows/Linux/macOS and also handles input.
- **Function pointers aren't available by default** — modern OpenGL functions must be loaded manually from the driver at runtime. **GLAD** generates the loader code for the exact OpenGL version/profile you target.

These two libraries are the foundation the next lesson builds on.
