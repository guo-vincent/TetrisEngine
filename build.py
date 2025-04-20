#!/usr/bin/env python3

import os
import shutil
import subprocess
from pathlib import Path

def main():
    # Define the build directory relative to the script's location
    repo_root = Path(__file__).resolve().parents[0]  # Always point to TetrisEngine/
    build_dir = repo_root / "build"
    vcpkg_root = os.environ.get('VCPKG_ROOT', 'C:')

    # Remove the build directory if it exists
    if build_dir.exists() and build_dir.is_dir():
        print("Removing existing build directory...")
        shutil.rmtree(build_dir)

    # Create a new build directory
    build_dir.mkdir()
    os.chdir(build_dir)

    # Define the CMake command
    cmake_command = [
        "cmake", "..",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DENABLE_NN=ON",
        "-DBUILD_TESTS=ON",
        f"-DCMAKE_TOOLCHAIN_FILE={vcpkg_root}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    ]

    # Run the CMake command
    print("Running cmake...")
    subprocess.run(cmake_command, check=True)

if __name__ == "__main__":
    main()
