# Software Renderer
With this project, I am trying to implement the GPU rendering pipeline in software to understand the steps deeper. This project can be used for learning purposes (I try to keep the code clean).

# Features

* Indexed triangle rasterization
* Depth testing
* Alpha blending
* Texture sampling with clamp and repeat address modes
* Custom shader support (via extending the Shader class)

# Showcase

The demonstration app is in the demo directory and it can be built for macOS (Xcode project or Makefile), Linux (Makefile) and Windows (Visual Studio project or Makefile). This is a sample output of the renderer (a box with one side transparent and another colored):
![SR sample](https://elviss.lv/files/sr_sample.png)