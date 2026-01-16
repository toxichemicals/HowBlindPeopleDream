#!/bin/bash

# This script installs all necessary Arch Linux dependencies for building
# the game binaries (Linux, Windows), excluding the Emscripten SDK (emsdk).

echo "--- Installing Arch Build Dependencies ---"
echo "You will be prompted for your sudo password to install packages via pacman."

# Check if sudo access is available
sudo -v
if [ $? -ne 0 ]; then
    echo "Error: Failed to obtain sudo privileges. Please ensure you can use sudo."
    exit 1
fi

# 1. Core GCC/MinGW Toolchains
# 2. Linux Static Dependencies (FIX for -lX11, -lGL linker errors)
#    These packages provide the static .a files needed for true static linking.
sudo pacman -S --needed \
    gcc-multilib \
    mingw-w64-gcc \
    tar \
    lib32-libx11 \
    lib32-libxcursor \
    lib32-libxi \
    lib32-libxrandr \
    lib32-mesa

if [ $? -eq 0 ]; then
    echo " "
    echo "Success! All non-Emscripten build dependencies are installed."
    echo "You now have the full set of tools for cross-platform static compilation."
    echo " "
else
    echo " "
    echo "Error: pacman failed to install all packages. Check the output above."
fi
