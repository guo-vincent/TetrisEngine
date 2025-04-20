#!/usr/bin/env python3

import os
import shutil
import subprocess
import sys
from pathlib import Path

def main():
    # Define the build directory relative to the script's location
    repo_root = Path(__file__).resolve().parents[0]  # Always point to TetrisEngine/
    build_dir = repo_root / "build"

    # Set VCPKG_ROOT based on the platform
    if sys.platform == "win32":
        vcpkg_root = os.environ.get('VCPKG_ROOT', 'C:\\path\\to\\vcpkg')
    else:
        vcpkg_root = os.environ.get('VCPKG_ROOT', str(Path.home() / 'vcpkg'))

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
        f"-DCMAKE_TOOLCHAIN_FILE={vcpkg_root}/scripts/buildsystems/vcpkg.cmake"
    ]

    # Run the CMake command
    print("Running cmake...")
    subprocess.run(cmake_command, check=True)

if __name__ == "__main__":
    main()
