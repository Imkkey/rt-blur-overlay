# RT Blur Overlay

This project demonstrates a minimal DirectX 11 motion blur overlay on Windows.
It builds with Visual Studio 2022 (v143 toolset) and the Windows SDK 10.0.22621.

## Folder Layout

- `src/main.cpp` – application source
- `src/BlurCS.hlsl` – compute shader implementing a simple EMA blur
- `build/BlurOverlay.vcxproj` – Visual Studio project (Debug|x64)

## Build

1. Open `build/BlurOverlay.vcxproj` in Visual Studio 2022.
2. Ensure the `x64` platform and `Debug` configuration are selected.
3. Build the project (Ctrl+Shift+B).

Running the resulting executable will create a transparent, click-through overlay
covering the primary monitor and apply a basic motion blur using the Desktop
Duplication API.
