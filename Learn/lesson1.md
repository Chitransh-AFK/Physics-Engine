# OpenGL Graphics Programming - Lesson 1: Getting Started

## Introduction to the Tools

### What is OpenGL?

**OpenGL** (Open Graphics Library) is a cross-platform, industry-standard API (Application Programming Interface) for rendering 2D and 3D graphics on the GPU (Graphics Processing Unit). It provides a powerful set of functions to:

- Draw primitive shapes (triangles, lines, points)
- Apply textures and lighting effects
- Manipulate 3D transformations
- Render to the screen efficiently

OpenGL abstracts hardware complexity, allowing developers to write graphics code that works across different GPUs and operating systems.

### What is GLFW?

**GLFW** (Graphics Library Framework) is a lightweight, open-source library that handles:

- **Window creation and management** - Creating and resizing windows
- **Input handling** - Keyboard, mouse, and joystick input
- **Context management** - Managing OpenGL rendering contexts
- **Cross-platform compatibility** - Working on Windows, macOS, and Linux

Without GLFW, you'd need to write platform-specific code for each operating system. GLFW abstracts these differences into a simple, unified API.

### What is GLAD?

**GLAD** (GL Auto Dispatch) is a loader library that:

- **Loads OpenGL function pointers** - Retrieves function addresses from your GPU driver
- **Handles version compatibility** - Manages different OpenGL versions and extensions
- **Provides type definitions** - Includes headers with proper data types and constants

Modern graphics cards support many OpenGL extensions, but the driver doesn't automatically provide all function addresses. GLAD handles this loading process, ensuring your code can call the latest OpenGL functions.

### Why Do We Use Them Together?

| Library | Purpose |
|---------|---------|
| **OpenGL** | The actual graphics API for rendering |
| **GLFW** | Window and context management |
| **GLAD** | Function loader for OpenGL |

**Analogy:** If OpenGL is the painter, GLFW is the canvas and easel, and GLAD is the tool that ensures all your brushes are properly connected and ready to use.

---

## Part 1: Initializing & Configuring GLFW

### Overview

Before rendering anything, we must initialize GLFW and configure it for OpenGL 3.3 with the **Core Profile**. This ensures we use modern OpenGL features and discard deprecated legacy functions.

### Step-by-Step

#### 1. Initialize GLFW

```cpp
glfwInit();
```

This function initializes the GLFW library. It must be called before any other GLFW functions. It sets up internal structures and prepares the library for use.

#### 2. Set OpenGL Version Hint

```cpp
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
```

These lines tell GLFW to request OpenGL version **3.3**:
- `MAJOR = 3` → Major version 3
- `MINOR = 3` → Minor version 3

Different graphics cards support different OpenGL versions. By specifying 3.3, we're requesting a version that's widely supported and provides modern features like shaders.

#### 3. Select Core Profile

```cpp
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
```

The **Core Profile** means:
- Only modern OpenGL functions are available
- Deprecated functions are disabled
- This prevents accidentally using outdated code
- Results in cleaner, more efficient programs

**Alternative:** `GLFW_OPENGL_COMPAT_PROFILE` includes legacy functions but should be avoided in new code.

### Why These Steps?

Setting hints before window creation ensures the GPU driver allocates the correct OpenGL context. If you don't specify the version or profile, you might get an incompatible or legacy context that limits your graphics capabilities.

---

## Part 2: Creating the Window

### Overview

Once GLFW is configured, we create an actual window that will display our graphics.

### Creating the Window

```cpp
GLFWwindow* window = glfwCreateWindow(800, 800, "LearnOpenGL", NULL, NULL);
```

**Parameters:**
- `800, 800` → Width and height of the window in pixels
- `"LearnOpenGL"` → Window title shown in the title bar
- `NULL, NULL` → Advanced parameters (monitor for fullscreen, shared context) - we don't use them here

**Return type:** `GLFWwindow*` is a pointer to a window structure containing:
- Window dimensions
- Input state
- OpenGL context information
- Other window properties

### Error Checking

```cpp
if(window == NULL){
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
}
```

Window creation can fail if:
- The GPU doesn't support the requested OpenGL version
- Not enough system resources are available
- The driver is outdated or incompatible

Always check if the window was created successfully before proceeding.

### Making the Context Current

```cpp
glfwMakeContextCurrent(window);
```

This tells OpenGL that all subsequent render calls should target this specific window. If you have multiple windows, you'd call this before rendering to each one to switch contexts.

### Why This Matters

The window is where all your graphics will be displayed. The OpenGL context is the state machine that tracks:
- The current viewport (rendering area)
- Active shaders and textures
- Blending modes and depth testing settings
- And much more

---

## Part 3: The Game Loop

### Overview

After creating the window, we enter a **game loop** (or render loop) that continuously:
1. Processes input events
2. Updates game state
3. Renders the scene
4. Displays the result

Without this loop, the window would appear, render once, and immediately close.

### The Loop Structure

```cpp
while(!glfwWindowShouldClose(window)){
    glfwPollEvents();  // Process events
    
    // TODO: Update game state
    // TODO: Render graphics
}
```

### How It Works

#### 1. Loop Condition: `!glfwWindowShouldClose(window)`

This function returns `true` if the user closes the window (e.g., clicks the X button), and `false` otherwise. The `!` (NOT operator) inverts this, so the loop continues while the window is **open**.

#### 2. Processing Events: `glfwPollEvents()`

```cpp
glfwPollEvents();
```

This function checks for any pending events like:
- Keyboard key presses
- Mouse movements and clicks
- Window resize events
- Window focus changes

Without calling this, events would pile up and the window would become unresponsive.

### Typical Loop Structure (for reference)

```cpp
while(!glfwWindowShouldClose(window)){
    // Input
    glfwPollEvents();
    
    // Update
    updateGameLogic();
    
    // Render
    clearScreen();
    drawObjects();
    swapBuffers();
}
```

### Why a Loop?

Modern graphics require continuous refreshing, typically 60+ times per second. The game loop enables:
- Smooth animation (update positions each frame)
- Responsive input (detect key presses immediately)
- Real-time interactivity
- Consistent frame rate

---

## Part 4: OpenGL Viewport & Buffers

### Overview

Before rendering, we need to understand how OpenGL manages screen output using buffers and viewports.

### Front and Back Buffers

OpenGL uses **double buffering** for smooth, flicker-free rendering:

```
┌─────────────────────────────────┐
│     Front Buffer (Displayed)    │  ← What you see on screen
└─────────────────────────────────┘
         ↕ (Swap every frame)
┌─────────────────────────────────┐
│      Back Buffer (Hidden)       │  ← Where we draw
└─────────────────────────────────┘
```

**Why Two Buffers?**
- While we're drawing to the **back buffer**, the **front buffer** displays the previous frame to the user
- This prevents the user from seeing incomplete or partial frames being drawn
- When done drawing, we **swap** the buffers, making the back buffer visible and the old front buffer the new drawing surface

**The Process:**
1. Frame 1: Draw to back buffer, front buffer shows previous frame
2. Swap buffers
3. Frame 2: Draw to new back buffer (old front), front buffer now displays frame 1
4. Swap buffers
5. Repeat...

### Setting the Viewport

```cpp
glViewport(x, y, width, height);
```

The **viewport** defines which rectangular area of the window OpenGL will render to.

**Example:**
```cpp
glViewport(0, 0, 800, 800);
```
- `0, 0` → Start at the bottom-left corner (OpenGL coordinate system origin)
- `800, 800` → Render to an 800×800 pixel area

**Why Viewport?**
- You can render to a portion of the window
- Useful for split-screen games or minimap displays
- Typically matches your window size

### Clearing the Screen

```cpp
glClearColor(0.1f, 0.1f, 0.3f, 1.0f);  // Set clear color (navy blue)
glClear(GL_COLOR_BUFFER_BIT);            // Clear the screen
```

**`glClearColor(R, G, B, Alpha)`:**
- Sets the color to fill the screen with each frame
- Values range from `0.0` to `1.0`
- `(0.1, 0.1, 0.3, 1.0)` creates a dark navy blue
- Called once during initialization (or when you want to change the clear color)

**`glClear(GL_COLOR_BUFFER_BIT)`:**
- Fills the entire viewport with the clear color
- Call this at the start of each frame before drawing
- Clears any residual pixels from the previous frame

**Other Clear Buffers:**
- `GL_COLOR_BUFFER_BIT` → Clear color buffer
- `GL_DEPTH_BUFFER_BIT` → Clear depth buffer (for 3D depth testing)
- `GL_STENCIL_BUFFER_BIT` → Clear stencil buffer (for advanced masking)

### Complete Example

```cpp
// In initialization:
glViewport(0, 0, 800, 800);
glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
// In game loop:
while(!glfwWindowShouldClose(window)){
    glClear(GL_COLOR_BUFFER_BIT);  // Clear screen with set color
    
    // Draw objects here
    
    glfwSwapBuffers(window);       // Swap buffers to display
    glfwPollEvents();
}
```

---

## Part 5: Displaying Color

### Overview

After clearing the screen, we need to display the chosen background color. This is done through **buffer swapping**.

### Buffer Swapping

```cpp
glfwSwapBuffers(window);
```

This function performs the double-buffer swap:

```
Before swap:           After swap:
┌─────────────────┐    ┌─────────────────┐
│ Back (Navy)     │ → │ Front (Navy)    │  Displayed to user
├─────────────────┤    ├─────────────────┤
│ Front (Old)     │ → │ Back (Old)      │  Ready for next frame
└─────────────────┘    └─────────────────┘
```

### The Complete Rendering Cycle

```cpp
while(!glfwWindowShouldClose(window)){
    // 1. Process input
    glfwPollEvents();
    
    // 2. Clear the screen with set color
    glClear(GL_COLOR_BUFFER_BIT);
    
    // 3. Draw your objects
    // (geometry, textures, effects, etc.)
    
    // 4. Display the result
    glfwSwapBuffers(window);
}
```

### Frame Timing

```
Frame 1:                Frame 2:                Frame 3:
Clear (navy) ──→        Clear (navy) ──→        Clear (navy) ──→
Draw objects ──→        Draw objects ──→        Draw objects ──→
Swap buffers ──→  Display navy   Swap buffers ──→  Display navy
```

Each frame:
1. **Clear** removes the old image
2. **Draw** adds new geometry/colors
3. **Swap** makes the new frame visible

The monitor refreshes (typically 60Hz) to display the front buffer. By swapping at the right time, you achieve smooth animation.

### Why This Matters

Without buffer swapping:
- You'd see partial frames (flickering)
- Objects would appear and disappear mid-frame
- Animation would look choppy and broken

With buffer swapping:
- Complete frames are always visible
- Smooth, consistent animation
- Professional visual quality

---

## Summary

| Stage | Code | Purpose |
|-------|------|---------|
| **Initialize GLFW** | `glfwInit()` | Set up the library |
| **Configure OpenGL** | `glfwWindowHint()` | Request OpenGL 3.3 Core Profile |
| **Create Window** | `glfwCreateWindow()` | Create rendering surface |
| **Make Context Current** | `glfwMakeContextCurrent()` | Target the window for rendering |
| **Game Loop** | `while(!glfwWindowShouldClose())` | Continuous rendering cycle |
| **Process Events** | `glfwPollEvents()` | Handle input and window events |
| **Set Viewport** | `glViewport()` | Define rendering area |
| **Set Clear Color** | `glClearColor()` | Choose background color |
| **Clear Screen** | `glClear()` | Fill screen with clear color |
| **Swap Buffers** | `glfwSwapBuffers()` | Display rendered frame |

This foundation prepares you for advanced topics like shaders, vertex buffers, and 3D transformations!
_