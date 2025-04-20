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

## Running Tests

After building, run the tests using CTest:

```bash
cd build
ctest
```

This will execute all tests located in the `tests/` directory.

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

For more information on integrating vcpkg with CMake, refer to the official documentation:

- [vcpkg CMake Integration](https://learn.microsoft.com/en-us/vcpkg/users/buildsystems/cmake-integration)

## Schema

TetrisEngine/                 Usage:
├── CMakeLists.txt            # Top-level CMake configuration
├── README.md                 # Project overview and setup instructions
├── build.py                  # Use this to make the file. Should work regardless of platform
├── setup_venv.py             # Helper function for setting up python venv, regardless of platform
├── .gitignore
├── docs/                     # Documentation and design notes
│   └── architecture.md       # High-level architecture diagram and explanations
├── include/                  # Public headers for the engine
│   └── TetrisEngine/         # Namespace folder
│       ├── Board.h           # Game board representation
│       ├── Piece.h           # Tetris piece definitions
│       ├── Engine.h          # Game loop and manager
│       └── NeuralNet.h       # NN wrapper interface
├── src/                      # Core C++ implementation
│   ├── main.cpp              # Entry point, command-line handling
│   ├── Board.cpp             # Board mechanics implementation
│   ├── Piece.cpp             # Piece handling code
│   ├── Engine.cpp            # Core game-engine logic
│   └── NeuralNet.cpp         # Neural network integration (loading/saving)
├── python/                   # Python scripts for training and evaluation
│   ├── data/                 # Data preprocessing and utilities
│   │   └── utils.py          # Dataset generation and augmentation
│   ├── models/               # Saved/trained model checkpoints
│   ├── train.py              # Training script (PyTorch/TensorFlow)
│   ├── evaluate.py           # Evaluation and benchmarking
│   ├── config.yaml           # Hyperparameter settings
│   └── requirements.txt      # Python import requirements
├── data/                     # Raw and processed game-play data
│   ├── raw/                  # Raw gameplay logs or recordings
│   └── processed/            # Processed feature vectors for training
├── models/                   # Exported neural network models for C++ inference
│   └── best_model.onnx       # Example ONNX model file
├── tests/                    # Unit and integration tests
│   ├── CMakeLists.txt        # Tests-specific CMake setup
│   ├── test_board.cpp        # GoogleTest suite for Board
│   ├── test_engine.cpp       # Tests for game logic
│   └── test_neuralnet.cpp    # Tests for NN interface & inference
└── scripts/                  # Utility scripts and CI helpers
│   ├── run_all_tests.sh      # Convenience script to build and run tests
│   └──download_data.sh       # Script to fetch or update training data
├── third_party/              # Contains linker files to ONNX runtime library
│   └── onnxruntime/
│       ├── include/
│       │   └── onnxruntime/
│       │       └── core/
│       │           └── session/
│       │               ├── onnxruntime_c_api.h
│       │               ├── onnxruntime_cxx_api.h
│       │               └── onnxruntime_cxx_inline.h
│       ├── lib/
│       │   ├── windows/
│       │   │   ├── onnxruntime.dll
│       │   │   └── onnxruntime.lib
│       │   ├── linux/
│       │   │   ├──libonnxruntime_providers_shared.so
│       │   │   ├── libonnxruntime.so
│       │   │   ├── libonnxruntime.so.1
│       │   │   └── libonnxruntime.so.1.18.1
│       │   └── mac/
│       │       ├── libonnxruntime.dylib
│       │       └── libonnxruntime.dylib.1.18.1
