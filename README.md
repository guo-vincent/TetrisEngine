# TetrisEngine

## THIS IS A WORK IN PROGRESS (This page will be updated incrementally)

README will be updated incrementally

---

## ABOUT

A modular Tetris engine with neural network integration, featuring:

- C++ core engine with ONNX Runtime support
- Python tools for training and evaluation
- Cross-platform build system using CMake and vcpkg

---

## Getting Started

### Prerequisites

Ensure the following tools are installed:

- CMake ≥ 3.15
- A C++17-compatible compiler
- Git
- Python ≥ 3.8 (for training scripts)

### Install vcpkg

vcpkg is used to manage C++ dependencies.

1. Clone the vcpkg repository:

   ```bash
   git clone https://github.com/microsoft/vcpkg.git
   ```

2. Navigate to the vcpkg directory and bootstrap:

   ```bash
   cd vcpkg
   ./bootstrap-vcpkg.sh  # Use .\bootstrap-vcpkg.bat on Windows
   ```

3. Add vcpkg to your system PATH (advanced environment variables)

   ```bash
   setx PATH "%PATH%;<path_to_vcpkg>"
   ```

   This allows you to use build.py.

4. Set VCPKG_ROOT environment variable to the root dir of your vcpkg installation:

   ```bash
   setx VCPKG_ROOT "<path_to_vcpkg>"
   ```

   Replace <path_to_vcpkg> with the actual path to your vcpkg directory.
   Make sure to do this in administrator mode, or it might not update!

### Clone TetrisEngine

Clone the repository and navigate to its directory:

```bash
git clone git@github.com:guo-vincent/TetrisEngine.git
cd TetrisEngine
```

---

## Building the Project

### Install Dependencies with vcpkg

Use vcpkg to install the necessary libraries:

```bash
./vcpkg/vcpkg install gtest
```

---

## Python Tools

### Setup Python Environment

Navigate to the `python/` directory and install the required Python packages:

```bash
cd python
pip install -r requirements.txt
```

Ensure that `requirements.txt` includes all necessary Python dependencies.

### Training and Evaluation

Use the provided scripts to train and evaluate your models:

- Train:

  ```bash
  python train.py
  ```

- Evaluate:

  ```bash
  python evaluate.py
  ```

---

## Compiling the program

- Run:

  ```bash
  python build.py
  ```

This executes the CMake file.

The executable will be under "build\bin\Release"
Test files will be generated under "build\bin\tests"

---

For more information on integrating vcpkg with CMake, refer to the official documentation:

- [vcpkg CMake Integration](https://learn.microsoft.com/en-us/vcpkg/users/buildsystems/cmake-integration)