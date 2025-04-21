cmake_minimum_required(VERSION 3.28)

if(NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    project(TetrisEngineTests LANGUAGES CXX)
endif()

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

# If ENABLE_NN is ON, make sure onnxruntime target exists from parent
if(ENABLE_NN AND NOT TARGET onnxruntime::onnxruntime)
    message(FATAL_ERROR "ONNX Runtime target not found. Make sure it's defined in the main CMake file.")
endif()

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/results")

set(TEST_SOURCES
    test_board.cpp
    test_engine.cpp
    test_neuralnet.cpp
)

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name "${test_src}" NAME_WE)
    
    add_executable("${test_name}" "${test_src}")
    
    target_link_libraries("${test_name}"
        PRIVATE
        TetrisEngineCore
        GTest::gtest
        GTest::gtest_main
        $<$<AND:$<BOOL:${ENABLE_NN}>,$<STREQUAL:"${test_name}",test_neuralnet>>:
            onnxruntime::onnxruntime
        >
    )
    
    gtest_discover_tests("${test_name}"
        EXTRA_ARGS "--gtest_output=xml:${CMAKE_CURRENT_BINARY_DIR}/results/${test_name}.xml"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    
    target_include_directories("${test_name}"
        PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
    )
endforeach()

if(ENABLE_NN)
  add_custom_command(TARGET test_neuralnet POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${ONNXRUNTIME_ROOT}/lib/$<LOWER_CASE:$<PLATFORM_ID>>/$<TARGET_FILE_NAME:onnxruntime::onnxruntime>"
      $<TARGET_FILE_DIR:test_neuralnet>
  )
endif()

# Specify runtime output directory for test executables
set_target_properties(test_board PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
)