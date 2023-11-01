# renderdoc_for_vulkanApp
this is a program for useing renderdoc to debug vulkan app demo





# note



## using VS2019 to compile 

the PCSample2_1.sln is 

```none
..\renderdoc_for_vulkanApp\src\PCSample2_1\build\PCSample2_1.sln
```



## modify the shader's path right in the file of PathData.h

```c++
// PathData.h
#ifndef PathData_H
#define PathData_H

#define PathPre "E:/code/renderdoc_for_vulkanApp/src/PCSample2_1/BNVulkanEx/"

#define VertShaderPath PathPre##"shaders/commonTexLight.vert.spv"
#define FragShaderPath PathPre##"shaders/commonTexLight.frag.spv"

#endif
```



## the app Compilation results "PCSample2_1.exe" 

in this folder

```
..\renderdoc_for_vulkanApp\src\PCSample2_1\bin\Debug\Vulkan_DPE\x64
```



# tips

> when you compile the app ,you must install [vulkan SDK ](https://www.lunarg.com/vulkan-sdk/)
