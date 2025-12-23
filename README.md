Project: `robot_demo` – Minimal hierarchical robot with one motion, camera, and basic lighting  

1) Toolchain & Libraries

- OpenGL Context: 3.3 Core (shaders use `version 330 core`)
- CMake: ≥ 3.10
- GLFW: 3.5.0 (bundled in `external/glfw`)
- GLM: 1.0.1 (bundled in `external/glm`)
- GLAD: bundled loader (in `external/glad`)

- All third‑party dependencies are vendored under `external/` so no system installs are required.

2) Build & Run (U of L Labs)

Copy‑paste the following in the project root (same folder as `CMakeLists.txt`):


mkdir build && cd build
cmake ..
make
./robot_demo


3) Working directory: the program expects to be launched from `build/`, because shaders are loaded via relative paths:

../resources/shaders/vertex_shader.glsl
../resources/shaders/fragment_shader.glsl

4) Controls

Action                                                        Key / Mouse                     
Move forward / back / left / right {only in free camera}      W / S / A / D             
Look around (yaw/pitch){only in free camera}                  Mouse move                  
Raise / lower right arm                                       ↑ / ↓(Up / Down arrows)
Change Environment                                            1: Default Ground (Bright Sky/Teal Ground). 2: Space Platform (Dark Background/Stars). 3: Jungle (Green Background/Bushes).
Toggle Camera Mode                                            F1: Free camera ; F2: Orbital camera
Quit                                                          Esc                 


5) Troubleshooting

Window opens but is black: Ensure you are launching from the `build/` directory so shader relative paths resolve; confirm `resources/shaders/*.glsl` exist one level above.

Linker errors about GL / GLU: Re-run `cmake ..` in a fresh `build/` (or `rm -rf build && mkdir build && cd build && cmake .. && make`).

6) Notes for Markers
Executable name: robot_demo