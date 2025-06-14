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
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Cross-platform warning configuration

# Configuration options
option(USE_GPU "Enable GPU acceleration for neural networks" OFF)
option(BUILD_TESTS "Build test executables" ON)
option(ENABLE_NN "Enable neural network integration" ON)
option(BUILD_DOC "Build documentation" OFF)

if(BUILD_DOC)
    find_package(Doxygen REQUIRED)
    
    # Create output directory
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs)

    set(DOXYFILE_IN  ${CMAKE_CURRENT_SOURCE_DIR}/docs/configFile.in)
    set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/docs/Doxyfile)
    configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

    add_custom_target(doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs
        COMMENT "Generating documentation"
        VERBATIM
    )
endif()

# ----------------------------------------------------------------------------
# Manual import of ONNX Runtime per-platform
# ----------------------------------------------------------------------------
if(ENABLE_NN)
    set(ONNXRUNTIME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/onnxruntime")
    add_library(onnxruntime::onnxruntime SHARED IMPORTED)

    # Determine CPU/GPU subdirectory
    # The following is configured with the ability to handle GPU args for futureproofing
    if(USE_GPU)
        set(ONNX_RUNTIME_LIB_SUBDIR "gpu")
    else()
        set(ONNX_RUNTIME_LIB_SUBDIR "cpu")
    endif()

    # Platform-specific paths
    if(WIN32)
        set_target_properties(onnxruntime::onnxruntime PROPERTIES
            IMPORTED_LOCATION   "${ONNXRUNTIME_ROOT}/lib/windows/${ONNX_RUNTIME_LIB_SUBDIR}/onnxruntime.dll"
            IMPORTED_IMPLIB     "${ONNXRUNTIME_ROOT}/lib/windows/${ONNX_RUNTIME_LIB_SUBDIR}/onnxruntime.lib"
            INTERFACE_INCLUDE_DIRECTORIES "${ONNXRUNTIME_ROOT}/include/core/session"
        )
    elseif(APPLE)
        set(CMAKE_MACOSX_RPATH ON)
        set_target_properties(onnxruntime::onnxruntime PROPERTIES
            IMPORTED_LOCATION   "${ONNXRUNTIME_ROOT}/lib/mac/${ONNX_RUNTIME_LIB_SUBDIR}/libonnxruntime.dylib"
            INTERFACE_INCLUDE_DIRECTORIES "${ONNXRUNTIME_ROOT}/include/core/session"
        )
    else()
        set_target_properties(onnxruntime::onnxruntime PROPERTIES
            IMPORTED_LOCATION   "${ONNXRUNTIME_ROOT}/lib/linux/${ONNX_RUNTIME_LIB_SUBDIR}/libonnxruntime.so"
            INTERFACE_INCLUDE_DIRECTORIES "${ONNXRUNTIME_ROOT}/include/core/session"
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
    src/Game.cpp
    src/UI.cpp
    src/UtilFunctions.cpp
)

if(MSVC)
    target_compile_options(TetrisEngineCore PRIVATE
        /W4
        # Uncomment this for strict
        # /WX
        /permissive-
        /sdl
        /Wall

        # suppress padding warning
        /wd4820
        # suppress unreferenced-inline removal
        /wd4514
        # suppress size_t→int and signed/unsigned conversion warnings
        /wd4267
        /wd4365
        /wd4710     # “function not inlined” (std::…::_Xrange, bad_array_new_length, etc.)
        /wd4711     # “function selected for automatic inline expansion”

        /external:anglebrackets 
        /external:W0
    )
else()
    # GCC/Clang
    target_compile_options(TetrisEngineCore PRIVATE
        -Wall -Wextra -Wpedantic
        -Wnon-virtual-dtor -Woverloaded-virtual 

        # suppress “padding added” in GCC/Clang
        -Wno-padded
        # suppress unused-inline warning
        -Wno-unused-function

        # suppress “size_t → int” and signed/unsigned mismatch
        -Wno-conversion
        -Wno-sign-conversion

        # suppress “could not inline” notes
        -Wno-inline
        -Wno-error=inline
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
        target_compile_options(TetrisEngineCore PRIVATE
            -Wall
        )
    endif()
endif()

target_include_directories(TetrisEngineCore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_subdirectory(third_party/raylib)
add_subdirectory(third_party/imgui)
add_subdirectory(third_party/rlImGui)

# ---------------------------------------------------------------
# Suppress warnings for third-party libraries
# ---------------------------------------------------------------
if(MSVC)
    # Disable all warnings for ImGui, Raylib, and rlImGui
    target_compile_options(imgui PRIVATE /W0)       # MSVC: disable all warnings
    target_compile_options(raylib PRIVATE /W0)
    target_compile_options(rlImGui PRIVATE /W0)
else()
    # GCC/Clang: suppress all warnings
    target_compile_options(imgui PRIVATE -w)        # -w = suppress all warnings
    target_compile_options(raylib PRIVATE -w)
    target_compile_options(rlImGui PRIVATE -w)
endif()

target_include_directories(imgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui")
target_include_directories(raylib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party/raylib/src")

if(ENABLE_NN)
    target_link_libraries(TetrisEngineCore PUBLIC onnxruntime::onnxruntime)
endif()

target_link_libraries(TetrisEngineCore PUBLIC raylib imgui rlImGui)

# Main executable
add_executable(TetrisEngine src/main.cpp)
if(WIN32)
    # Compile the resource file into an object file
    enable_language(RC)  # Ensure CMake recognizes .rc files
    target_sources(TetrisEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/appicon/appicon.rc)
endif()
target_link_libraries(TetrisEngine PRIVATE TetrisEngineCore)

# Copy the correct ONNX Runtime library post-build -- only if windows
if(WIN32)
    add_custom_command(TARGET TetrisEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:onnxruntime::onnxruntime>"
            $<TARGET_FILE_DIR:TetrisEngine>
    )
endif()

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

    # ======================================================
    # Add CoreML/Metal GPU support when requested
    # ======================================================
    if(USE_GPU)
        message(STATUS "Enabling macOS GPU acceleration (CoreML/Metal)")
        find_library(ACCELERATE_LIB Accelerate)
        find_library(COREML_LIB CoreML)
        target_link_libraries(TetrisEngineCore PRIVATE 
            ${ACCELERATE_LIB} 
            ${COREML_LIB}
        )
    endif()
endif()

set_target_properties(TetrisEngine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)