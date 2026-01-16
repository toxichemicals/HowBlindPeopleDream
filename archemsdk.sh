#!/bin/bash

# Configuration
EMSDK_DIR="emsdk_portable"
EMSDK_REPO="https://github.com/emscripten-core/emsdk.git"

# --- 1. Arch Linux Confirmation ---
echo "This script is targeted for the Arch Linux installation, continue? Y/N"
read -r CONFIRMATION

if [[ "$CONFIRMATION" =~ ^[Yy]$ ]]; then
    echo "Starting emsdk setup for project directory..."
else
    echo "Installation canceled by user."
    exit 0
fi

# --- 2. Check for Git ---
if ! command -v git &> /dev/null
then
    echo "Error: 'git' command not found. Please install git (e.g., sudo pacman -S git) and run the script again."
    exit 1
fi

# --- 3. Clone or Update emsdk ---
if [ -d "$EMSDK_DIR" ]; then
    echo "Found existing emsdk directory. Updating and activating..."
    cd "$EMSDK_DIR" || exit
    git pull
else
    echo "Cloning emsdk into '$EMSDK_DIR'..."
    git clone "$EMSDK_REPO" "$EMSDK_DIR"
    cd "$EMSDK_DIR" || exit
fi

# --- 4. Install and Activate the Latest SDK ---

# Install the latest stable version
echo "Installing the latest stable Emscripten SDK..."
./emsdk install latest

# Activate the environment in this directory
echo "Activating emsdk environment locally (will be sourceable via '$EMSDK_DIR/emsdk_env.sh')..."
./emsdk activate latest

echo " "
echo "Emscripten SDK Setup Complete!"
echo " "
echo "To use 'emcc' in your current shell, run the following command:"
echo "source $PWD/emsdk_env.sh"
echo " "

cd .. # Go back to the project root
# The environment sourcing is left to the user or the calling build script.
