# RT Blur Overlay

An experimental scaffold for a DirectX 11 motion-blur overlay on Windows.
The project targets Visual Studio 2022 (v143 toolset) and Windows SDK 10.0.22621.0.

The overlay is incomplete: compute-shader resource binding and presentation of
the blurred output are not implemented.

## Folder Layout

- `src/main.cpp`: window setup, desktop capture, and compute-shader scaffold.
- `src/BlurCS.hlsl`: four-frame weighted-blend compute shader.
- `build/BlurOverlay.vcxproj`: Visual Studio project.

## Build

Open `build/BlurOverlay.vcxproj` in Visual Studio 2022 with the C++ tools and
Windows SDK 10.0.22621.0 installed. The project defines the `Debug | x64` configuration.
