# TetrisEngine

## THIS IS A WORK IN PROGRESS (This page will be updated incrementally)

README will be updated incrementally
Currently, ONNX Runtime is only tested (and known to work on Windows-CPU)
Other versions may or may not work. Needs more testing. Then I'll figure it out.

Need to test:

1. Windows-CPU (WORKS!)
2. Mac-CPU
3. Linux-CPU

GPU files contain nothing. This is because they are not yet implemented and are not a priority right now.

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

- CMake ≥ 3.15 - [Installation Guide](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Getting%20Started.html#getting-and-installing-cmake-on-your-computer)
- A C++20-compatible compiler
- Git
- Python ≥ 3.8 (for training scripts)

### Installing CMake

#### Linux

##### Ubuntu/Debian

sudo apt-get update
sudo apt-get install cmake

#### Windows

1. Download the latest installer from [cmake.org/download](https://cmake.org/download/)
2. Run the installer and select:
   - "Add CMake to the system PATH for all users"
   - "Create CMake Desktop Icon" (optional)

#### Mac

##### Using Homebrew

```cmd
brew install cmake
```

##### Downloaded directly (not recommended)

```cmd
curl -OL [https://github.com/Kitware/CMake/releases/download/v3.28.3/cmake-3.28.3-macos-universal.tar.gz](https://github.com/Kitware/CMake/releases/download/v3.28.3/cmake-3.28.3-macos-universal.tar.gz)
tar -xzf cmake-*.tar.gz
sudo mv cmake-*.app /Applications/
```

#### Verify Installation

```cmd
cmake --version
```

You should see a message like

```bash
cmake version 4.0.1

CMake suite maintained and supported by Kitware "(kitware.com/cmake)".
```

### Install vcpkg

Downloading CMake is a prerequisite for vcpkg.

Make sure CMake is downloaded before proceeding further.

vcpkg is used to manage C++ dependencies.

1. Clone the vcpkg repository:

   ```bash
   git clone https://github.com/microsoft/vcpkg.git
   ```

2. Navigate to the vcpkg directory and bootstrap:

   ```bash
   cd vcpkg
   ./bootstrap-vcpkg.sh  # .\bootstrap-vcpkg.bat on Windows
   ```

   If you do not want to follow step 3 and onwards, place vcpkg in the system root.
   That is, UNDER:

   - Windows: "C:\Users\YourName"
   - Mac: "/Users/YourName"
   - Linux: "/home/YourName"

   There is no gurantee that this will work with build.py.

   I HIGHLY RECOMMEND just saving the variables to PATH.

3. Add vcpkg to your system PATH (advanced environment variables)

   WARNING: For steps 3-4 you may need to access terminal/powershell in administrator mode for PATH to update.

   ```bash
   setx PATH "%PATH%;<path_to_vcpkg>"
   ```

   This allows you to use build.py.

4. Set VCPKG_ROOT environment variable to the root dir of your vcpkg installation:

   ```bash
   setx VCPKG_ROOT "<path_to_vcpkg>"
   ```

   Replace <path_to_vcpkg> with the actual path to your vcpkg directory.

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

Confirm your installation:

```bash
vcpkg list
```

You should see this message:

```bash
gtest:x64-`<platform>`
```

where `<platform>` represents your computer's architecture.

---

## Python Tools

### Setup Python Environment

Navigate to the TetrisEngine/ directory

run

```bash
python setup_venv.py
```

This sets up venv for you. Pretty nice, since I always forget the venv command.

Follow the instructions that correspond with your computer architecture.

If no error messages show up, you're done. Keep reading.
If not, you are probably missing something major (like python) and I'll assume you know how to set python up.

### Training and Evaluation

Train.py and evaluate.py ARE NOT YET IMPLEMENTED AND CURRENTLY DO NOTHING!

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

THIS IS IMPLEMENTED

Navigate to TetrisEngine/

- Run:

  ```bash
  python build.py
  ```

This executes the CMake file.

The executable will be under "build\bin\Release"
Test files will be generated under "build\bin\tests"

To make your life easier, build.py takes in some optional flags:

1. "--tests": compiles unit tests. ON by default.
2. "--no-tests": tells CMake to skip compilation of tests.
   Cannot be declared with the --tests flag.
   Shortens compilation time.

3. "--run": immediately executes all files compiled by CMake.
   This includes unit tests compiled, if they compiled successfully.

The output generated can get quite verbose, so I recommend piping the output to a .txt file.

Just make sure to edit .gitignore to ignore the file.

---

For more information on integrating vcpkg with CMake, refer to the official documentation:

- [vcpkg CMake Integration](https://learn.microsoft.com/en-us/vcpkg/users/buildsystems/cmake-integration)
