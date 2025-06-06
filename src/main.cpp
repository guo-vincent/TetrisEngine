#include <iostream>

// Testing imports
#include <onnxruntime_cxx_api.h>
#include "../include/TetrisEngine/Board.h"
#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>

using namespace tetris;

std::string PieceTypeToString(PieceType pt) {
    switch (pt) {
        case PieceType::I: return "I";
        case PieceType::O: return "O";
        case PieceType::T: return "T";
        case PieceType::S: return "S";
        case PieceType::Z: return "Z";
        case PieceType::J: return "J";
        case PieceType::L: return "L";
        default: return " ";
    }
}

Color GetColorForPiece(PieceType pt) {
    static std::unordered_map<PieceType, Color> colorMap = {
        {PieceType::EMPTY, DARKGRAY},
        {PieceType::I, SKYBLUE},
        {PieceType::O, YELLOW},
        {PieceType::T, PURPLE},
        {PieceType::S, GREEN},
        {PieceType::Z, RED},
        {PieceType::J, BLUE},
        {PieceType::L, ORANGE},
    };
    return colorMap.count(pt) ? colorMap[pt] : RAYWHITE;
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

    // list of previous commands
    std::vector<std::string> commandHistory;

    std::mt19937 local_rng(std::random_device{}());
    
    // Pass RNG to board constructor
    Board board(local_rng);

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
    const int boardOffsetX = 100;
    const int boardOffsetY = 100;

    

    // main loop add game stuff here
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        rlImGuiBegin();
        // offset to prevent overlap from squares

        // this is box for controlling without keybinds
        ImGui::SetNextWindowPos(ImVec2(800, 100));
        ImGui::Begin("Controls");

        if (!gameOver) {
            // these controls were also reversed
            if (ImGui::Button("Left") || IsKeyPressed(KEY_LEFT)) {
                board.MoveActivePiece(-1, 0);
                commandHistory.push_back("Move Left");
            }
            ImGui::SameLine();
            if (ImGui::Button("Right") || IsKeyPressed(KEY_RIGHT)){ 
                board.MoveActivePiece(1, 0);
                commandHistory.push_back("Move Right");
            }
            
            if (ImGui::Button("Soft Drop") || IsKeyPressed(KEY_DOWN)) {
                commandHistory.push_back("Drop 1");
                if (!board.MoveActivePiece(0, -1)) {
                    board.LockActivePiece();

                    if (!board.SpawnRandomPiece()) gameOver = true;
                    board.GetNextQueue();

                   
                }
            }
            
            ImGui::SameLine();  
            if (ImGui::Button("Hard Drop") || IsKeyPressed(KEY_SPACE)) {
                board.HardDropActivePiece();
                commandHistory.push_back("Drop X");
                if (!board.SpawnRandomPiece()) gameOver = true;
                board.GetNextQueue();
            }
            ImGui::SameLine();
            if (ImGui::Button("HOLD") || IsKeyPressed(KEY_LEFT_SHIFT)) {
                board.HoldPiece();
                commandHistory.push_back("Hold Piece");
            }

            if (ImGui::Button("Rotate CW") || IsKeyPressed(KEY_E)){ 
                board.RotateActivePiece(RotationDirection::CLOCKWISE);
                commandHistory.push_back("Rotate CW");
            }
            ImGui::SameLine();
            if (ImGui::Button("Rotate CCW") || IsKeyPressed(KEY_Q)) {
                board.RotateActivePiece(RotationDirection::COUNTER_CLOCKWISE);
                commandHistory.push_back("Rotate CCW");
            }
            ImGui::SameLine();
            if (ImGui::Button("Rotate 180") || IsKeyPressed(KEY_W)) {
                board.RotateActivePiece(RotationDirection::ONE_EIGHTY);
                commandHistory.push_back("Rotate 180");
            }

            if (ImGui::Button("Reset") || IsKeyPressed(KEY_T)) {
                board.Reset();
                commandHistory.push_back("Reset Board");
            }
           
           // erase first command if bigger than 10
            if (commandHistory.size() > 10) {
                commandHistory.erase(commandHistory.begin());
            }
        }

        if (gameOver) {
            ImGui::Text("Game Over!");
        }    
        
        ImGui::Text("Score: %d", board.GetScore());
        ImGui::Text("Lines: %d", board.GetLinesCleared());
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(450, 100));
        ImGui::SetNextWindowSize(ImVec2(100, 0));
        ImGui::Begin("Queue");
        for (PieceType pt : board.GetNextQueue()) {
            Color c = GetColorForPiece(pt);
            ImGui::ColorButton("##color", ImVec4{c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, 1.0f},
                       ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop,
                       ImVec2(30, 30));
            ImGui::SameLine();
            ImGui::TextUnformatted(PieceTypeToString(pt).c_str());
        }
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(450, 500));
        ImGui::SetNextWindowSize(ImVec2(100, 0));
        ImGui::Begin("Hold");
        
        PieceType held = board.GetHeldPieceType();
        if (held != PieceType::EMPTY) {
            Color c = GetColorForPiece(held);
            ImGui::ColorButton("##heldcolor", 
                ImVec4{c.r/255.0f, c.g/255.0f, c.b/255.0f, 1.0f},
                ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop,
                ImVec2(30, 30));
            ImGui::SameLine();
            ImGui::TextUnformatted(PieceTypeToString(held).c_str());
        } else {
            ImGui::Text("Empty");
        }
        ImGui::End();

        // this is the box to see command history
        ImGui::SetNextWindowPos(ImVec2(800, 400));
        ImGui::Begin("Command History");
        // populate
        ImGui::BeginChild("ScrollRegion", ImVec2(250, 200), true, 0);
        for (const std::string& cmd : commandHistory) {
            ImGui::TextUnformatted(cmd.c_str());
        }
        ImGui::EndChild();

        rlImGuiEnd();

        // Draw board
        std::vector<PieceType> state(VISIBLE_BOARD_HEIGHT * BOARD_WIDTH, PieceType::EMPTY);
        state = board.GetRenderableState();

        for (int row = 0; row < VISIBLE_BOARD_HEIGHT; ++row) {
            for (int col = 0; col < BOARD_WIDTH; ++col) {
                PieceType pt = state[row * BOARD_WIDTH + col];
                Color color = GetColorForPiece(pt);

                DrawRectangle(
                    boardOffsetX + col * cellSize,
                    boardOffsetY + row * cellSize,
                    cellSize - 2,
                    cellSize - 2,
                    color
                );
            }
        }

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    std::cout << "All graphics libraries closed successfully.\n";

    return 0;
}