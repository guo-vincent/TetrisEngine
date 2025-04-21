#include <iostream>

// This is the relative path to onnxruntime. DO NOT MESS WITH THIS, OR YOU WILL BREAK SOMETHING.
#include <../third_party/onnxruntime/include/core/session/onnxruntime_cxx_api.h>

int main() {
    // All this does is test to make sure onnxruntime is importing properly.
    try {
        // Initialize the ONNX Runtime environment
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "TetrisEngineTest");

        // Create session options
        Ort::SessionOptions session_options;
        session_options.SetIntraOpNumThreads(1);

        std::cout << "ONNX Runtime environment and session options initialized successfully." << std::endl;
    } catch (const Ort::Exception& e) {
        std::cerr << "ONNX Runtime initialization failed: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Hello, Tetris Engine!" << std::endl;
    return 0;
}