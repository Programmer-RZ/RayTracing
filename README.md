# RayTracing

RayTracing editor. Extremely lightweight! Seriously, this could run on oven!

Heavily in development.

Written entirely in C++.

![image](https://github.com/Programmer-RZ/RayTracing/assets/123858154/7793102c-4f5d-411e-aa0e-4fe740a48ec3)

## Requirements

### Compile

- Visual Studio 2022 (other versions have not been tested)
- Vulkan SDK

### System

Currently only supports Windows. Preferably, Windows 11 since it has been tested.

## Features

### Raytracer

- Shading
- Path Tracing
- Accumulation

### GUI

- Material/Scene Editor
- Settings/Options panel

### Materials

- Lambertians
- Metal
- Diffuse light

### Objects

- Sphere
- Box

### Optimizations

- Ray coherence
- Reduced bounces
- Renders new image when updated objects/position
- Reduce image quality in scene editor

## Dependencies

- Walnut
- mINI
- stb
- nativefiledialog

## Examples

### Dark room

![test1](https://github.com/Programmer-RZ/RayTracing/assets/123858154/ca89569c-ccb9-4a23-a366-1ab6aad7c8e1)

### Bright sky

![test](https://github.com/Programmer-RZ/RayTracing/assets/123858154/faa8491c-d295-46fc-b5e5-e3f517cfae1c)

## Credits

- Thanks to [RayTracing Tutorials](https://www.youtube.com/watch?v=gfW1Fhd9u9Q&list=PLlrATfBNZ98edc5GshdBtREv5asFW3yXl)
- Thanks to [Raytracing in One Weekend Series](https://raytracing.github.io/)
