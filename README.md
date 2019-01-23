#Vulkan Tutorial

This is completly based on this video tutorial: [klick](https://www.youtube.com/watch?v=mzVFHEmnRLg&index=1&list=PL58qjcU5nk8uH9mmlASm4SFy1yuPzDAH0), all hail goes to this guy. I can also recommend [this](https://vulkan-tutorial.com).

### General

Since I build this completely in Linux I can't assure it will run on Windows or OSX. But shouldn't be too hard to modify the project to your OS.

#### Dependencies
In order to build and run this, you'll need the following libraries and SDKs. As mentioned above this was built on Linux, specifically on ArchLinux, so I can provide hints only for this distribution:
* A Vulkan compatible graphics card and drivers. Refer to your OS and vendor for how-to install them. For ArchLinux it should be this:
  * ```$ sudo pacman -S vulkan-intel``` for Intel graphics (in my case)
  * ```$ sudo pacman -S vulkan-radeon``` for Radeon cards
  * Nvidia should have Vulkan support in their proprietary driver, which should be ```$ sudo pacman -S nvidia-utils```.
* The Vulkan SDK ```$ sudo pacman -S vulkan-devel```
* [GLFW Library](https://www.glfw.org/) ```$ sudo pacman -S glfw-x11```
* [GLSL](https://www.khronos.org/registry/OpenGL/index_gl.php) ```$ sudo pacman -S glslang```

#### Build and run

* Clone the project:

```git clone https://github.com/tylernewnoise/VulkanTutorial.git```
* Build it:
 ```
 $ cd VulkanTutorial
 $ mkdir build
 $ cd build
 $ cmake ..
 $ make
```
* Run it:

```$ ./VulkanTut```
