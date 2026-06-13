# Physics Engine from Scratch

## My Journey 🚀

I discovered a passion for low-level, performance-critical code while scrolling through YouTube. Watching countless creators build physics engines from scratch sparked something in me. I had recently learned C++, and thought to myself: **"Why not make myself suffer and build one too?"**

And here we are.

This repository documents my adventure into graphics programming and physics simulation. What started as curiosity has become an exercise in understanding how the digital world actually works beneath the layers of abstraction we usually rely on.

## About This Project

This is a **from-scratch physics engine** built with:
- **C++** for performance and low-level control
- **OpenGL** for graphics rendering
- **GLFW** for window management
- **GLAD** for OpenGL function loading

**Important Note:** The code in this project is **written entirely by hand**—no AI code generation. This is built from:
- YouTube tutorials and educational videos
- Wiki documentation and technical references
- My own understanding and experimentation
- Countless bugs and debugging sessions

The lessons in the `Learn/` folder are created with AI assistance to help organize and explain concepts clearly, but every line of engine code is mine.

## Project Structure

```
physics_engine/
├── src/
│   ├── main.cpp          # Entry point and main engine loop
│   └── glad.c            # OpenGL function loader
├── include/              # Library headers
│   ├── glad/
│   ├── GLFW/
│   └── KHR/
├── lib/                  # Pre-compiled libraries
├── Learn/                # Educational lessons and explanations
│   └── ....              # Fundamentals: OpenGL, GLFW, GLAD
└── README.md             # This file
```

## Learning Resources

I've created a series of lessons in the `Learn/` folder to document what I've learned along the way. These lessons cover:

## My Learning Path

1. **Discover** — Found tutorials on physics engines and graphics programming
2. **Learn C++** — Covered the basics of the language
3. **Understand Graphics** — Dove into OpenGL, shaders, and rendering concepts
4. **Build** — Started implementing core engine components
5. **Document** — Created lessons to solidify understanding and help others

## What's Next?

Future additions to this engine:
- [x] Rendering a simple window with title and color
- [x] Rendering 2D shapes and meshes
- [ ] Basic physics simulation (gravity, collision detection)
- [ ] Input handling and camera controls
- [ ] Shader programming
- [ ] More advanced physics (rigid bodies, constraints)
- [ ] Performance optimization

## Why This Matters

In a world of high-level frameworks and engines, building from scratch teaches you:
- **How graphics actually work** — Not just calling pre-built functions
- **Performance consciousness** — Understanding trade-offs and optimization
- **Debugging skills** — Figuring out what went wrong at the lowest levels
- **Pride in creation** — You understand every line of code

## Special Thanks

- **YouTube creators** whose tutorials provided guidance and inspiration
- **Documentation and wiki resources** that answered my questions
- **The C++ community** for maintaining excellent resources
- **My determination** for pushing through the suffering 😄
- **Copilot** For Documentation

## License

This project is personal and educational. Feel free to learn from it, but please respect the work and provide attribution if you use any code.

---

**Last Updated:** June 11, 2026

**Status:** 🔨 **In Development** — This engine is actively being built and refined.

---

*"The best way to learn is to do. And sometimes, doing means making yourself suffer through a physics engine from scratch."* — Me, questioning my life choices while debugging a seg fault at 2 AM.
