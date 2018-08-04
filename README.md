# Software Renderer
With this project, I am trying to implement the GPU rendering pipeline in software to understand the steps deeper. This project can be used for learning purposes (I try to keep the code clean).

# Features

* Indexed triangle rasterization
* Depth testing
* Alpha blending
* Texture sampling with clamp, repeat, and mirror address modes
* Custom shader support (via extending the Shader class)
* Point and linear texture filtering

# Usage

Just include the headers from the sr directory in your project and build. You can check the code in the demo directory and build the demo project to see how to use the library.

# Showcase

The demonstration app is in the demo directory and it can be built for macOS (Xcode project or Makefile), Linux/Sun Solaris (Makefile, use gmake to build it on Sun Solaris), Windows (Visual Studio project or Makefile) and Haiku (Makefile). This is a sample output of the renderer (a box with one side transparent and another colored):
![SR sample](https://elviss.lv/files/sr_sample_filtered.png)