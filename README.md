# Pebble Watchface

A custom Pebble watchface project built using the official Pebble SDK and `pebble-tool`.

This project can be built and tested locally using **WSL (Windows Subsystem for Linux)** or any Linux/macOS environment that supports the Pebble SDK.

---

## Requirements

### For Windows users
Make sure you have:
- **Windows 11** (recommended, for native WSLg GUI support)
- **WSL with Ubuntu** installed:
  ```bash
  wsl --install
- **Visual Studio Code with the Remote – WSL extension** (to edit from Windows and run inside Ubuntu)
- **Git** installed on both Windows and Ubuntu

### For MacOS users
Make sure you have:
- **Homebrew** (if not already installed)

## Initial Setup

## For Ubuntu / Windows (using WSL) 
### Update packages
    sudo apt update && sudo apt install -y
    curl git unzip python3 python3-venv python3-pip build-essential \
    libsdl2-2.0-0 libpixman-1-0 qemu-system-arm libgl1 \
    libffi-dev libssl-dev

### Install uv (Python tool manager)
    curl -LsSf https://astral.sh/uv/install.sh | sh
    echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
    source ~/.bashrc

### Install Pebble tool and SDK
    uv tool install pebble-tool
    pebble sdk install latest

### Verify installation
    pebble --version
    pebble sdk list

## MacOS
### Install Homebrew (if not already installed)
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

### Install dependencies
    brew install python3 git wget unzip qemu sdl2

### Install Pebble sdk via uv
    curl -LsSf https://astral.sh/uv/install.sh | sh
    echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.zshrc
    source ~/.zshrc

    uv tool install pebble-tool
    pebble sdk install latest

### Verify installation
    pebble --version
    pebble sdk list



## How to run the project
### Clone the repository:

    git clone https://github.com/MatiPendino/pebble-watchface.git
    cd pebble-watchface

### Build the watchface
    pebble build

### Start the emulator
    pebble emu start basalt

### Install the app
    pebble logs --emulator basalt

## Tips
- The compiled .pbw file appears in the build/ folder.
- Test other watch models:
    ```
    pebble emu start aplite   # Pebble Classic
    pebble emu start basalt   # Pebble Time
    pebble emu start chalk    # Pebble Time Round
    pebble emu start emery    # Pebble Time 2