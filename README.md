# IFNITY Rendering Engine Tool  

**IFNITY** is a tool designed for **real-time rendering** and **rapid prototyping of models**.  
What makes it unique is its multi-API support for **OpenGL, Vulkan, and DirectX 12**, with upcoming support for **WebGPU**.  
Currently, API switching is available for **window context creation**. In the future, **runtime API switching for scene generation** will be implemented to provide seamless transitions across different graphics APIs during execution.  


<img width="1263" height="722" alt="image" src="https://github.com/user-attachments/assets/50651bbf-80e8-437f-b64c-d27cc6516c32" />

## API Configuration Notes

Currently, it is not required to build or have all three APIs (Vulkan, DirectX 12, OpenGL) enabled at once.
From CMake, you can configure which graphics APIs you want to include in the build.
If your system does not support a particular API, it will be automatically disabled during configuration.

The available examples have been prepared to demonstrate bindless rendering under both Vulkan and DirectX 12.
The RHI (Rendering Hardware Interface) usage between Vulkan and D3D12 is almost identical, with separation into different source files mainly due to shader differences.
OpenGL, however, uses a different RHI flow, which is implemented separately.

At the moment, no new features are being added, but the engine will continue to receive improvements and updates over time.

##  Architecture Overview

### CORE
- **Modular and extensible architecture** for R&D and prototyping
- **API abstraction layer** – unified interface across graphics APIs
- **Scene management** with Data-Oriented Design (DOD) for cache efficiency
- **Asset pipeline** for model loading and texture processing
- **Planned: DLL separation** for better modularity and hot-reloading

### RENDERING HARDWARE INTERFACE (RHI)

#### Vulkan 1.3 and Support New Vulkan 1.4321
- ✅ **Dynamic rendering** (VK_KHR_dynamic_rendering)
- ✅ **Bindless resources** (descriptor indexing)
- ✅ **Multi-draw indirect** commands
- ✅ **Timeline semaphores** for efficient synchronization
- ✅ **Memory allocation** with VMA
- ✅ **Scene caching** with DOD patterns
  ✅ **SRIV Cross** compatibily with Vulkan 1.4321 


#### DirectX 12 (SM 6.6)
- ✅ **Dynamic rendering** pipeline
- ✅ **Bindless resources** (descriptor heaps)
- ✅ **Multi-draw indirect** commands
- ✅ **Efficient synchronization** with timeline semaphores
- ✅ **Resource barriers** optimization
- ✅ **Scene caching** with DOD patterns


#### OpenGL 4.6
- ✅ **Bindless textures** (ARB_bindless_texture)
- ✅ **Multi-draw indirect** (ARB_multi_draw_indirect)
- ✅ **Direct State Access** (DSA)
- ✅ **Efficient buffer management**
- ✅ **Scene caching** with DOD patterns




### FEATURES STATUS
- **API switching for window contexts** – flexible graphics API selection
- **Planned: Runtime API switching for scene generation** (future enhancement)
- **Internal benchmarking tools** for API performance comparison
- [![Build Status](https://github.com/alfonsmagd/IFNITY-ENGINE/actions/workflows/cmake-single-platform.yml/badge.svg)](https://github.com/alfonsmagd/IFNITY-ENGINE/actions)

---

## Bindless Rendering 
<img width="952" height="380" alt="image" src="https://github.com/user-attachments/assets/d244f6a0-d958-4a45-b10f-39ff1eb6e63a" />

## Vulkan vs DirectX 12 Comparative (Frame-Level)

The engine includes internal benchmarking tools to analyze API behavior at frame level.  
Below is an example capture comparing Vulkan and D3D12 performance:

<p align="center">
  <img width="1678" height="1081" src="https://github.com/user-attachments/assets/65a52c4d-f655-4bf2-886b-657152ee1b5f" alt="Vulkan vs D3D12 comparative"/>
</p>

---

##  Build Instructions

### Prerequisites
- **Visual Studio 2022** (with CMake support)
- **Git** (for submodules)
- **Vulkan SDK** (latest version)
- **Windows 10/11** (DirectX 12 support)

### Getting Started

**1. Clone the repository with submodules:**
```bash
git clone --recursive https://github.com/alfonsmagd/IFNITY-ENGINE.git
```

**2. Open with Visual Studio:**
- Open the folder in Visual Studio 2022
- CMake will automatically configure with Ninja build system
- This provides a more user-friendly solution generation

<img width="1278" height="322" alt="image" src="https://github.com/user-attachments/assets/49e8e63f-8301-4f23-9de9-9e5707f47a2c" />

**3. Select target examples:**
- **D3D12Scene** - DirectX 12 rendering example
- **sceneVk** - Vulkan rendering example  
- **scene_materials** - OpenGL 4.6 rendering example

<img width="495" height="542" alt="image" src="https://github.com/user-attachments/assets/9073619c-f428-4cce-8856-74158c128da5" />

##  Some Result Comparative 



<img width="1355" height="663" alt="image" src="https://github.com/user-attachments/assets/5bad6fbb-d88d-405a-b537-02b5ec7b617a" />


<img width="1223" height="350" alt="image" src="https://github.com/user-attachments/assets/6a801ef3-2baa-4e7d-9dd2-3840a3e36c6f" />
