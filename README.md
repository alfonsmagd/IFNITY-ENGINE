# IFNITY Rendering Engine Tool  

**IFNITY** is a tool designed for **real-time rendering** and **rapid prototyping of models**.  
What makes it unique is its ability to **switch between OpenGL, Vulkan, and DirectX 12 at runtime**, with upcoming support for **WebGPU**.  
This provides developers with seamless transitions across different graphics APIs — **without restarting or recompiling the project**.  

[![Build Status](https://github.com/alfonsmagd/IFNITY-ENGINE/actions/workflows/cmake-single-platform.yml/badge.svg)](https://github.com/alfonsmagd/IFNITY-ENGINE/actions)

---

##  Features
- Multi-API Rendering: **OpenGL 4.6**, **Vulkan 1.3**, **DirectX 12 (SM 6.6)**.  
- **Runtime API switching** – no restart, no recompilation.  
- Modern rendering techniques: **dynamic rendering, bindless resources, timeline semaphores**.  
- Modular and extensible architecture for **R&D and prototyping**.  
- Upcoming support for **WebGPU**.

---

## Vulkan vs DirectX 12 Comparative (Frame-Level)

The engine includes internal benchmarking tools to analyze API behavior at frame level.  
Below is an example capture comparing Vulkan and D3D12 performance:

<p align="center">
  <img width="1678" height="1081" src="https://github.com/user-attachments/assets/65a52c4d-f655-4bf2-886b-657152ee1b5f" alt="Vulkan vs D3D12 comparative"/>
</p>

---

##  Build Instructions

Clone the repository **with submodules**:


git clone --recursive https://github.com/alfonsmagd/IFNITY-ENGINE.git


Open CMake here with Visual Studio so that it uses Ninja by default and generates the solution in a more user-friendly way.
<img width="1278" height="322" alt="image" src="https://github.com/user-attachments/assets/49e8e63f-8301-4f23-9de9-9e5707f47a2c" />

