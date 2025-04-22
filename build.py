import os
import shutil
import subprocess
import sys
from pathlib import Path
import argparse

def main():
    parser = argparse.ArgumentParser()
    
    # use gpu (non-functional by design)
    parser.add_argument('--use-gpu', action='store_true', help='Enable GPU support for ONNX Runtime')
    
    # include tests? Speeds up compilation a lot, but won't run unit tests.
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--tests', action='store_true', dest='build_tests',
                      help='Enable building test executables (default)')
    group.add_argument('--no-tests', action='store_false', dest='build_tests',
                      help='Disable building test executables')
    parser.set_defaults(build_tests=False)
    
    # run files right after compilation?
    parser.add_argument('--run', action='store_true', help='run files immediately after compilation')
    
    # update doxygen documents?
    parser.add_argument('--document', action='store_true', dest='document', help='update doxygen files')
    
    # use cache?
    parser.add_argument('--use-cache', action='store_true', dest='use_cache', help='Reuse existing build directory (skip deletion)')
        
    args = parser.parse_args()
    
    # Define the build directory relative to the script's location
    repo_root = Path(__file__).resolve().parents[0]  # Always point to TetrisEngine/
    build_dir = repo_root / "build"

    vcpkg_root = os.environ.get('VCPKG_ROOT', str(Path.home() / 'vcpkg'))

    # Remove the build directory if it exists. Disabled if --use-cache is set
    if not args.use_cache:
        if build_dir.exists() and build_dir.is_dir():
            print("Removing existing build directory...")
            try:
                shutil.rmtree(build_dir)
            except PermissionError as e:
                print(f"Failed to delete {build_dir}: {e}")
                print("Close any open files/Explorer windows")
                print("Your antivirus may also cause problems")
                sys.exit(1)

    if not build_dir.exists():
        build_dir.mkdir()

    os.chdir(build_dir)

    # Define the CMake configuration command
    cmake_configure_command = [
        "cmake", "..",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DENABLE_NN=ON",
        f"-DBUILD_TESTS={'ON' if args.build_tests else 'OFF'}",
        f"-DUSE_GPU=OFF",
        f"-DCMAKE_TOOLCHAIN_FILE={vcpkg_root}/scripts/buildsystems/vcpkg.cmake",
        f"-DBUILD_DOC={'ON' if args.document else 'OFF'}",
        "-DCMAKE_POLICY_VERSION_MINIMUM=3.28"
    ]

    # Run the CMake configuration command
    print("Configuring the project with CMake...")
    subprocess.run(cmake_configure_command, check=True)

    # Define the CMake build command
    cmake_build_command = [
        "cmake", "--build", ".", "--config", "Release"
    ]

    # Run the CMake build command
    print("Building the project with CMake...")
    subprocess.run(cmake_build_command, check=True)
    
    # --run flag processing. Basically it'll only process what has compiled properly
    if args.run:
        exe_suffix = ".exe" if sys.platform == "win32" else ""
        main_exe_name = f"TetrisEngine{exe_suffix}"
        test_exe_names = [
            f"test_board{exe_suffix}",
            f"test_engine{exe_suffix}", 
            f"test_neuralnet{exe_suffix}"
        ]

        # Platform-specific path construction
        if sys.platform == "win32":
            bin_path = build_dir / "bin" / "Release"
            test_bin_path = build_dir / "bin" / "tests" / "Release"
        else:
            bin_path = build_dir / "bin"
            test_bin_path = build_dir / "bin" / "tests"

        # Run main executable
        main_exe = bin_path / main_exe_name
        if main_exe.exists():
            print(f"\n Running main executable: {main_exe}\n\n")
            subprocess.run([str(main_exe)], cwd=str(bin_path), check=True)
        else:
            print(f" Main executable not found at {main_exe}")

        # Run tests if enabled
        if args.build_tests:
            print("\n\nRunning tests...")
            for test_name in test_exe_names:
                test_exe = test_bin_path / test_name
                if test_exe.exists():
                    print(f"\nRunning test: {test_name}")
                    subprocess.run([str(test_exe)], cwd=str(test_bin_path), check=False)
                else:
                    print(f"Test executable not found: {test_exe}")
    

if __name__ == "__main__":
    main()
