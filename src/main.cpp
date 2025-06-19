#include <iostream>

// Testing imports
#include <onnxruntime_cxx_api.h>
#include "TetrisEngine/Board.h"
#include "TetrisEngine/Game.h"
#include "TetrisEngine/Ui.h"
#include "TetrisEngine/Controls.h"
#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>

using namespace tetris;

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

    // list of previous commands
    std::vector<std::string> commandHistory0;
    std::vector<std::string> commandHistory1;
    
    // Pass RNG to board constructor
    Game game(2);

    // Raylib + ImGui
    int monitor = GetCurrentMonitor();
    int screenWidth = GetMonitorWidth(monitor);
    int screenHeight = GetMonitorHeight(monitor);

    InitWindow(screenWidth, screenHeight, "Tetris Engine - GUI Test");
    // test FPS, set back to 60 if need
    SetTargetFPS(144);
    rlImGuiSetup(true);

    // window borders
    bool gameOver0 = false;
    bool gameOver1 = false;
    const int cellSize = 30;
    const int board0OffsetX = 100;
    const int board1OffsetX = 1000;
    const int boardOffsetY = 100;

    // main loop add game stuff here
    while (!WindowShouldClose()) {
        game.Update();

        gameOver0 = game.getBoard(0).IsGameOver();
        if (!gameOver0){
            if (IsKeyPressed(KEY_LEFT)) ControlLeft(game.getBoard(0));
            if (IsKeyPressed(KEY_RIGHT)) ControlRight(game.getBoard(0));
            if (IsKeyPressed(KEY_E)) ControlRotateCW(game.getBoard(0));
            if (IsKeyPressed(KEY_Q)) ControlRotateCCW(game.getBoard(0));
            if (IsKeyPressed(KEY_W)) ControlRotate180(game.getBoard(0));
            if (IsKeyPressed(KEY_SPACE)) ControlHardDrop(game.getBoard(0));
            if (IsKeyPressed(KEY_DOWN)) ControlSoftDrop(game.getBoard(0));
            if (IsKeyPressed(KEY_LEFT_SHIFT)) ControlHold(game.getBoard(0));
        }
        if (IsKeyPressed(KEY_T)) game.Reset();

        BeginDrawing();
        ClearBackground(BLACK);

        rlImGuiBegin();

        DrawPlayer(game, 0, board0OffsetX, boardOffsetY, commandHistory0, gameOver0, cellSize);
        DrawPlayer(game, 1, board1OffsetX, boardOffsetY, commandHistory1, gameOver1, cellSize);      
        
        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    std::cout << "All graphics libraries closed successfully.\n";

    return 0;
}