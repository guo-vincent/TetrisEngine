#include <iostream>

// Testing imports
#include <onnxruntime_cxx_api.h>
#include "../include/TetrisEngine/Board.h"
#include "../include/TetrisEngine/Game.h"
#include "../include/TetrisEngine/Ui.h"
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
    bool gameOver = false;
    const int cellSize = 30;
    const int board0OffsetX = 100;
    const int board1OffsetX = 1000;
    const int boardOffsetY = 100;

    // main loop add game stuff here
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        // rlImGuiBegin();

        // gameOver = DrawControlsPanel(game.getBoard(0), commandHistory0, gameOver);

        // DrawQueuePanel(game.getBoard(0));
        // DrawHoldPanel (game.getBoard(0));
        // DrawHistoryPanel(commandHistory0);

        gameOver = DrawPlayer(game, 0, board0OffsetX, boardOffsetY, commandHistory0, gameOver, cellSize);
        gameOver = DrawPlayer(game, 1, board1OffsetX, boardOffsetY, commandHistory1, gameOver, cellSize);
        
        // rlImGuiEnd();

        // // draw the actual tetris board
        // DrawBoardGrid(game.getBoard(0), board0OffsetX, boardOffsetY, cellSize);
        // DrawBoardGrid(game.getBoard(0), 1000, boardOffsetY, cellSize);
      
        

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    std::cout << "All graphics libraries closed successfully.\n";

    return 0;
}