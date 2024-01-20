# Project Koala

**Koala** is custom game engine based on Vk&Python. It uses Vulkan to render scene, and Python to control gameplay.

# Getting Started

THIS PROJECT REQUIRES CMAKE VER >= 3.27.

You will be required to install CMake and Python3 in your system. 

AFTER CMake is installed correctly, Install the Package Manager called 'vcpkg'. See [here](https://vcpkg.io/en/getting-started).

Install the following packages through VCPKG:

`vcpkg install glfw3 eigen3 spdlog fmt vulkan-memory-allocator`

And configure VCPKG to your cmake via the following commandline:

`-DCMAKE_TOOLCHAIN_FILE=YOUR_vcpkg\scripts\buildsystems\vcpkg.cmake`

By the default, the Vulkan RHI(Rendering Hardware Interface) is enabled. (The only RHI for now, haha)