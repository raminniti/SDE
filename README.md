# SEED: SDL Environmental Engine & Developer-tool

SEED is a high-performance, lightweight 2D retro game framework built from scratch in C/C++ using the SDL2 library, paired with a companion graphical level editor developed in Python and Pygame. 

Designed with a strict focus on memory efficiency and low-level performance, the engine rejects heavy object-oriented abstractions in favor of a procedural, Data-Oriented Design (DOD). It utilizes pre-allocated contiguous memory pools to optimize CPU cache locality and completely eliminates runtime fragmentation.

## 🚀 Key Features

### 🛠️ Runtime C/C++ Game Engine
* **Data-Oriented Memory Management:** All game entities are allocated at initialization within a single contiguous array. Active entity destruction is handled via an $O(1)$ pop-and-swap routine, shrinking iteration loops dynamically.
* **Branchless Tile Animation:** Eliminates runtime CPU conditional branching (if-statements) inside high-frequency rendering loops by utilizing bitmask flag multipliers to compute global tile animation frame offsets.
* **Kinematic Nudge Physics:** Implements smooth grid-based cardinal movement equipped with a corner-nudging algorithm to prevent bounding-box friction in tight spatial environments.
* **Two-Stage Framebuffer Scaling:** Renders to a low-resolution offscreen surface before upscale centering on modern hardware, cleanly preserving retro pixel art aspect ratios.

### 🎨 Standalone Python/Pygame Level Editor
* **Rapid UI Prototyping:** Built using an Object-Oriented design paradigm in Python to facilitate fast tool construction without adding executable overhead to the core engine runtime.
* **Direct Binary Serialization:** Visually updates structural tiles and absolute entity spawn placements, serializing data directly into the engine's custom sequential binary map format (`.map`).

---

## 🎮 Main Execution Loop Architecture

The application entry point utilizes an immutable main loop lifecycle, decoupling platform-dependent hardware tracking from variable game states.

```cpp
#include "src/engine.h"
#include "src/game.h"

int main(int argc, char* args[]) {
    EngineInit("SEED Engine Framework", 1280, 720);
    GameInit();

    while (EngineIsRunning()){
        EngineUpdate();         // Process platform input and OS events
        GameUpdate();           // Execute physics calculations and entity logic
        
        EngineRenderStart();    // Clear graphics backbuffer
        GameDraw();             // Issue batched draw commands
        EngineRenderEnd();      // Present final frame to the hardware display
    }

    GameQuit();
    EngineQuit();
    return 0;
}
```

---

## 🛠️ Compilation & Requirements

### Dependencies
* **Engine Core:** C/C++ Compiler, SDL2, SDL_image, SDL_mixer
* **Editor Tooling:** Python 3.11+, Pygame library

### Zero-Configuration Out-of-the-Box Build
The project structure encapsulates the required SDL2 developer frameworks locally using relative indirect paths. No global operating system or IDE environment variables are required to evaluate the codebase.

1. Clone the repository to your local directory.
2. Open the solution file using **** (or newer).
3. Ensure the build configuration is set to match your machine targets (x86/x64).
4. Hit **Build & Run**. The paths resolve automatically inside the local project context.

*For manual environment modifications or platform setups, refer to the [Lazy Foo' SDL2 Setup Documentation](https://lazyfoo.net/tutorials/SDL/01_hello_SDL/index.php).*

---

## 🕹️ Control Mappings

### Core Game Runtime
* **Movement:** `W`, `A`, `S`, `D` (Cardinal Direction Constraints)
* **Action/Attack:** `Spacebar`

### Level Editor Toolkit

| Action | Input Command |
| :--- | :--- |
| **Room Navigation** | Arrow Keys |
| **Toggle Object Mode** | `Tab` Key |
| **Select & Paint Tiles** | Left Mouse Button (`LMB`) |
| **Copy Active Tile** | Right Mouse Button (`RMB`) |
| **Tile Sheet Browsing** | Scroll Wheel |
| **Global Tile Replacement** | `R` Key |
| **Copy / Paste Grid Chunk** | `C` Key / `V` Key |
| **Floor Increment / Decrement** | `Page Up` / `Page Down` |
| **Serialize Level Array to Disk** | `S` Key |
| **Parse Level Binary to Grid** | `L` Key |

---

## 👤 Author
* **Robert (Tony) Minniti Jr.**
