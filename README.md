# Project Koala

**Koala** is custom game engine based on Vk&Python. It uses Vulkan to render scene, and Python to control gameplay.

NOTE Koala is still in early developement.

# Getting Started

By the default, the Vulkan RHI is enabled (And this is the only RHI we supported now). 

Please follow 'Quick Start' to setup your developement environment. Only Windows is supported for now. 

# Quick Start (Windows)

First, you need to install the following softwares:

- CMake. Required CMake ver >= 3.25.

- Python3.

- Git.

- Vulkan SDK.

After cloned this project to your PC, please run Setup.bat to sync all needed third-party libs.

And then, run GenerateProjectFiles.bat to generate project files.

After project files are generated, you can find Koala.sln file in `Build` directory. Use VisualStudio or Rider to open it to start development.

# Quick Start (Linux, only Ubuntu is supported now)

NOTE: Currently, this project is only compiled with Ubuntu 24.04LTS. So the following building instruction for Ubuntu dist only.

Before you build this engine, you need make sure you have installed the following packages (via apt):

```
build-essential git libgl-dev libpython3.12-dev libwayland-dev libxkbcommon-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl-dev
```

And download latest VulkanSDK from [LunarG's website](https://vulkan.lunarg.com/sdk/home#linux)

The latest version of VulkanSDK(1.3.280.1) has been verified can be used to build. Please download and unzip it.

And open a shell, execute the following command to setup build environment:

```bash
source {PathToYourVulkanSDK}/1.3.280.1/setup-env.sh
```

In the **same** shell, execute the following command in Koala repo:

```bash
bash ./Setup.sh && bash ./GenerateProjectFiles.sh
```

then, you can enter the 'koala/Build' directory and start the build by `make`.

On the same shell, you can open your IDE to development (like CLion).
# Before you start to development...

Please read CODERULES.md for CodingStyles.

# FAQ

(TBD)