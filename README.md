# Vulkan Tutorial

This is heavily based on [this tutorial](https://vulkan-tutorial.com), all hail goes to this guy. It loads a 3D Model from a obj-file, puts a texture on it and applies [Phong Shading](https://en.wikipedia.org/wiki/Shading#Phong_shading).

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
* [GLM](https://glm.g-truc.net/0.9.9/index.html ) ```$ sudo pacman -S glm```
 
#### Build and run
* Clone the project:

```git clone https://github.com/tylernewnoise/VulkanTutorial.git```
* Build the shaders:
```
$ cd VulkanTutorial
$ cd data/shaders
$ ./compileShaders.sh
```
* Build the application:
 ```
 $ cd VulkanTutorial
 $ mkdir build
 $ cd build
 $ cmake ..
 $ make
```

* Run it:

```$ ./VulkanTut```

If everything went well you should see the earth.
![galaxy_texure](screenshot.png)

* Interactions:
    * l turns the light on/off.
    * spacebar starts/stops rotation
    * up/down moves the camera to/away from object

#### Accomplishments

The example has implemented the following features:
* loading a 3D model
* loading a texture
* loading vertex and fragment shaders
* resizable window
* depth buffering
* [Phong shading](https://en.wikipedia.org/wiki/Phong_shading)
* [Mipmaps](https://en.wikipedia.org/wiki/Mipmap)
* [MultiSample AntiAliasing](https://en.wikipedia.org/wiki/Multisample_anti-aliasing)
* [Sample Shading](https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#primsrast-sampleshading)
* [Normal Maps](https://en.wikipedia.org/wiki/Normal_mapping) (buggy)

#### Bugs
* Some memory leakage when closing the application
* Something is wrong with the Normal Mapping as it has a blueish light

#### Credits, resources, inspirations

* [vulkan tutorial](https://vulkan-tutorial.com)
* brotcrunshers [youtube tutorial](https://www.youtube.com/watch?v=mzVFHEmnRLg&index=1&list=PL58qjcU5nk8uH9mmlASm4SFy1yuPzDAH0)
* [stb](https://github.com/nothings/stb) image library
* [tinyobjloader](https://github.com/syoyo/tinyobjloader) object loader
* [Sascha Willems](https://github.com/SaschaWillems/Vulkan)
* [gun model](https://free3d.com/3d-model/45-acp-smith-and-wesson-13999.html)
* [earth model](https://free3d.com/3d-model/planet-earth-99065.html)
* [vulkan cookbook](https://github.com/PacktPublishing/Vulkan-Cookbook)

### TODO

* fix bugs
* http://kylehalladay.com/blog/tutorial/vulkan/2017/08/13/Vulkan-Uniform-Buffers.html
* PushConstants
* 