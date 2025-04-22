# TetrisEngine

A modular, cross-platform Tetris engine with neural network integration.

[TOC]

...

## Overview

TetrisEngine provides:

- C++ core with ONNX Runtime
- Python training utilities
- Cross-platform build via CMake/vcpkg

## Status

### Work in Progress

- Currently verified on Windows (CPU only).
- Support for Mac/Linux and GPU backends is under development.

---

## About

TetrisEngine is a modular Tetris engine featuring:

- **C++ Core** with ONNX Runtime support for AI-driven gameplay
- **Python Utilities** for training and evaluation
- **Cross-Platform Build** using CMake and vcpkg

---

## Prerequisites

Before you begin, ensure you have:

- **CMake** ≥ 3.28
- A **C++20**-compatible compiler (MSVC, GCC, Clang)
- **Git**
- **Python** ≥ 3.8 (for training scripts)

---

## Installation

### Install CMake {#cmake}

#### Ubuntu / Debian (CMake)

```bash
sudo apt-get update
sudo apt-get install cmake
```

#### macOS Installation (Homebrew)

```bash
brew install cmake
```

#### Windows (Doxygen)

1. Download the installer from <https://cmake.org/download>
2. Run the installer and choose:
   - "Add CMake to the system PATH for all users"
   - (Optional) "Create CMake Desktop Icon"

Verify:

```bash
cmake --version
```

Expected output:

```bash
cmake version 4.0.1
CMake suite maintained and supported by Kitware (kitware.com/cmake)
```

### Install Doxygen {#install-doxygen}

Doxygen generates API documentation from annotated source code.

#### Ubuntu / Debian

```bash
sudo apt-get update
sudo apt-get install doxygen doxygen-doc doxygen-gui graphviz
```

#### macOS (Homebrew)

```bash
brew install doxygen graphviz
```

#### Windows

1. Download the latest Doxygen installer from <https://www.doxygen.nl/download.html>
2. (Optional) Install Graphviz from <https://graphviz.org/download/>
3. Run the Doxygen installer and follow the wizard.

Verify:

```bash
doxygen --version
```

### Install vcpkg {#install-vcpkg}

vcpkg manages C++ dependencies.

```bash
# Clone the repo
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg

# Bootstrap (Linux/macOS)
./bootstrap-vcpkg.sh

# or Windows
# .\\bootstrap-vcpkg.bat
```

Add vcpkg to your PATH and set `VCPKG_ROOT`:

```bash
# Example (bash):
export VCPKG_ROOT=~/vcpkg
export PATH="$VCPKG_ROOT:$PATH"
```

---

## Cloning the Repository {#cloning}

```bash
git clone git@github.com:guo-vincent/TetrisEngine.git
cd TetrisEngine
```

---

## Building the Project

### Dependencies via vcpkg {#dependencies-via-vcpkg}

Install required libraries:

```bash
vcpkg install gtest
```

Verify:

```bash
vcpkg list
# Expect: gtest:x64-<platform>
```

### Compiling & Flags {#compiling-flags}

Use the provided build script:

```bash
python build.py [options]
```

#### Available Flags

- `--no-tests` (default): Skip unit tests
- `--tests`: Compile unit tests
- `--run`: Run all compiled binaries
- `--document`: Regenerate Doxygen docs
- `--use-cache`: Incremental build (may skip docs/tests)

**Note:** On large builds, redirect output to a file and add it to `.gitignore`.

**Output Paths**:

- **Linux/macOS**: `build/bin/` and `build/bin/tests`
- **Windows**: `build/bin/Release/` and `build/bin/tests/Release/`

---

## Python Tools {#python-tools}

Set up a virtual environment and install dependencies:

```bash
python setup_venv.py
```

> **Note**: `train.py` and `evaluate.py` are not yet implemented.

Examples:

```bash
python train.py
python evaluate.py
```

---
