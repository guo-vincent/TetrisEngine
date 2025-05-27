#include <iostream>

// Testing imports
#include <onnxruntime_cxx_api.h>
#include "../include/TetrisEngine/Board.h"
#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>

using namespace tetris;

void test_game() {
    Board board;
    bool game_over = false;
    std::string input_line;

    if (!board.SpawnRandomPiece()) {
        game_over = true;
    }

    while (!game_over) {
        board.PrintBoardText(false);

        std::cout << "Command (q/e/a/d/s/x/p): ";
        if (!std::getline(std::cin, input_line)) {
            // Handle input errors or EOF (e.g., Ctrl+D)
            game_over = true;
            break;
        }

        if (input_line.empty()) {
            continue; // Skip empty input
        }

        char input = input_line[0]; // Take first character only

        switch (input) {
            case 'a':
                board.MoveActivePiece(-1, 0);
                break;
            case 'd':
                board.MoveActivePiece(1, 0);
                break;
            case 's':
                if (!board.MoveActivePiece(0, -1)) {
                    board.LockActivePiece();
                    if (!board.SpawnRandomPiece()) {
                        game_over = true;
                    }
                }
                break;
            case 'e':
                board.RotateActivePiece(RotationDirection::CLOCKWISE);
                break;
            case 'q':
                board.RotateActivePiece(RotationDirection::COUNTER_CLOCKWISE);
                break;
            case 'x':
                board.HardDropActivePiece();
                if (!board.SpawnRandomPiece()) {
                    game_over = true;
                }
                break;
            case 'p':
                game_over = true;
                break;
            default:
                std::cout << "Invalid command.\n";
                break;
        }

        if (board.IsGameOver()) {
            game_over = true;
        }
    }

    std::cout << "Game Over! Score: " << board.GetScore() 
              << ", Lines Cleared: " << board.GetLinesCleared() << "\n";
}

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
    test_game();
    return 0;

}