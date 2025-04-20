cmake_minimum_required(VERSION 3.28)

# Allow vcpkg toolchain path override via environment variable
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project(TetrisEngine
    LANGUAGES CXX
    VERSION 0.1.0
    DESCRIPTION "Tetris game engine with neural network integration"
)

# Platform-agnostic compiler settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Cross-platform warning configuration
if(MSVC)
    add_compile_options(/W4 /WX /permissive-)
else()
    add_compile_options(
        -Wall -Wextra -Wpedantic -Werror
        -Wnon-virtual-dtor -Woverloaded-virtual -Wconversion
        $<$<CXX_COMPILER_ID:Clang,AppleClang>:-Weverything -Wno-c++98-compat>
    )
endif()

# Configuration options
option(USE_GPU "Enable GPU acceleration for neural networks" OFF)
option(BUILD_TESTS "Build test executables" ON)
option(ENABLE_NN "Enable neural network integration" ON)

# ----------------------------------------------------------------------------
# Manual import of ONNX Runtime per-platform
# ----------------------------------------------------------------------------
if(ENABLE_NN)
    set(ONNXRUNTIME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/onnxruntime")

    # Create an imported target for the ONNX Runtime library
    add_library(onnxruntime::onnxruntime SHARED IMPORTED)

    # Configure it differently depending on the platform
    if(WIN32)
        # Windows: link against onnxruntime.dll + onnxruntime.lib
        set_target_properties(onnxruntime::onnxruntime PROPERTIES
            IMPORTED_LOCATION   "${ONNXRUNTIME_ROOT}/lib/windows/onnxruntime.dll"
            IMPORTED_IMPLIB     "${ONNXRUNTIME_ROOT}/lib/windows/onnxruntime.lib"
            INTERFACE_INCLUDE_DIRECTORIES "${ONNXRUNTIME_ROOT}/include"
        )
    elseif(APPLE)
        # macOS: link against libonnxruntime.dylib
        set(CMAKE_MACOSX_RPATH ON)  # embed @rpath so the loader finds the .dylib
        set_target_properties(onnxruntime::onnxruntime PROPERTIES
            IMPORTED_LOCATION   "${ONNXRUNTIME_ROOT}/lib/mac/libonnxruntime.dylib"
            INTERFACE_INCLUDE_DIRECTORIES "${ONNXRUNTIME_ROOT}/include"
        )
    else()  # UNIX (Linux)
        # Linux: link against libonnxruntime.so
        set_target_properties(onnxruntime::onnxruntime PROPERTIES
            IMPORTED_LOCATION   "${ONNXRUNTIME_ROOT}/lib/linux/libonnxruntime.so"
            INTERFACE_INCLUDE_DIRECTORIES "${ONNXRUNTIME_ROOT}/include"
        )
    endif()
endif()

# ----------------------------------------------------------------------------
# Core library configuration
# ----------------------------------------------------------------------------
add_library(TetrisEngineCore STATIC
    src/Board.cpp
    src/Piece.cpp
    src/Engine.cpp
    src/NeuralNetwork.cpp
)

target_include_directories(TetrisEngineCore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(ENABLE_NN)
    target_link_libraries(TetrisEngineCore PRIVATE onnxruntime::onnxruntime)
endif()

# Main executable
add_executable(TetrisEngine src/main.cpp)
target_link_libraries(TetrisEngine PRIVATE TetrisEngineCore)

# Installation targets (cross-platform)
install(TARGETS TetrisEngine TetrisEngineCore
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)

# ----------------------------------------------------------------------------
# Testing subsystem
# ----------------------------------------------------------------------------
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Platform-specific post-configuration
if(UNIX AND NOT APPLE)
    # Linux-specific settings
    find_package(Threads REQUIRED)
    target_link_libraries(TetrisEngineCore PRIVATE Threads::Threads)
elseif(APPLE)
    # macOS-specific settings
    set(CMAKE_MACOSX_RPATH ON)
endif()