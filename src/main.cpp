#include <iostream>

// Testing imports
#include <onnxruntime_cxx_api.h>
#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>

int main() {
    // All this does is test to make sure onnxruntime is importing properly.
    try {
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "TetrisEngineTest");

        Ort::SessionOptions session_options;
        session_options.SetIntraOpNumThreads(1);

        std::cout << "ONNX Runtime environment and session options initialized successfully." << std::endl;
    } catch (const Ort::Exception& e) {
        std::cerr << "ONNX Runtime initialization failed: " << e.what() << std::endl;
        return 1;
    }

    // Testing all 3 graphics libararies
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "Tetris Engine - Graphics Test");
    SetTargetFPS(60);

    rlImGuiSetup(true);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        rlImGuiBegin();

        ImGui::Begin("Debug Panel");
        ImGui::Text("Hello, ImGui!");
        ImGui::Text("FPS: %.1f", GetFPS());
        if (ImGui::Button("Test Button")) {
            std::cout << "Button clicked!\n";
        }
        ImGui::End();

        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();

    std::cout << "All graphics libraries closed successfully.\n";
    return 0;

}