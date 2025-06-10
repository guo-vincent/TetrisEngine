// UI.cpp
#include "../include/TetrisEngine/Ui.h"
#include "../include/TetrisEngine/Board.h"
#include "../include/TetrisEngine/Game.h"
#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>

namespace tetris {

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

bool DrawControlsPanel(Game& game,
                       std::vector<std::string>& commandHistory,
                       bool gameOver,
                       const ImVec2& SetNextWindowPosVector
                    )
{
    ImGui::SetNextWindowPos(SetNextWindowPosVector);
    ImGui::Begin("Controls");
    
    if (!gameOver) {
        // these controls were also reversed
        if (ImGui::Button("Left") || IsKeyPressed(KEY_LEFT)) {
            game.getBoard(0).MoveActivePiece(-1, 0);
            commandHistory.push_back("Move Left");
        }
        ImGui::SameLine();
        if (ImGui::Button("Right") || IsKeyPressed(KEY_RIGHT)){ 
            game.getBoard(0).MoveActivePiece(1, 0);
            commandHistory.push_back("Move Right");
        }
        if (ImGui::Button("Soft Drop") || IsKeyPressed(KEY_DOWN)) {
            commandHistory.push_back("Drop 1");
            if (!game.getBoard(0).MoveActivePiece(0, -1)) {
                game.getBoard(0).LockActivePiece();

                if (!game.getBoard(0).SpawnRandomPiece()) gameOver = true;
                game.getBoard(0).GetNextQueue();

                
            }
        }
        ImGui::SameLine();  
        if (ImGui::Button("Hard Drop") || IsKeyPressed(KEY_SPACE)) {
            game.getBoard(0).HardDropActivePiece();
            commandHistory.push_back("Drop X");
            if (!game.getBoard(0).SpawnRandomPiece()) gameOver = true;
            game.getBoard(0).GetNextQueue();
        }
        ImGui::SameLine();
        if (ImGui::Button("HOLD") || IsKeyPressed(KEY_LEFT_SHIFT)) {
            game.getBoard(0).HoldPiece();
            commandHistory.push_back("Hold Piece");
        }
        if (ImGui::Button("Rotate CW") || IsKeyPressed(KEY_E)){ 
            game.getBoard(0).RotateActivePiece(RotationDirection::CLOCKWISE);
            commandHistory.push_back("Rotate CW");
        }
        ImGui::SameLine();
        if (ImGui::Button("Rotate CCW") || IsKeyPressed(KEY_Q)) {
            game.getBoard(0).RotateActivePiece(RotationDirection::COUNTER_CLOCKWISE);
            commandHistory.push_back("Rotate CCW");
        }
        ImGui::SameLine();
        if (ImGui::Button("Rotate 180") || IsKeyPressed(KEY_W)) {
            game.getBoard(0).RotateActivePiece(RotationDirection::ONE_EIGHTY);
            commandHistory.push_back("Rotate 180");
        }

        if (ImGui::Button("Reset") || IsKeyPressed(KEY_T)) {
            game.getBoard(0).Reset();
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

    ImGui::Text("Score: %d", game.getBoard(0).GetScore());
    ImGui::Text("Lines: %d", game.getBoard(0).GetLinesCleared());
    ImGui::End();

    return gameOver;
}

void DrawQueuePanel(const Board& board,
                    const ImVec2& SetNextWindowPosVector,
                    const ImVec2& SetNextWindowSizeVector) {
    ImGui::SetNextWindowPos(SetNextWindowPosVector);
    ImGui::SetNextWindowSize(SetNextWindowSizeVector);
    ImGui::Begin("Queue");
    for (PieceType pt : board.GetNextQueue()) {
        Color c = GetColorForPiece(pt);
        ImGui::ColorButton("##color",
            {c.r/255.f,c.g/255.f,c.b/255.f,1.f},
            ImGuiColorEditFlags_NoTooltip|ImGuiColorEditFlags_NoDragDrop,
            {30,30});
        ImGui::SameLine();
        ImGui::TextUnformatted(PieceTypeToString(pt).c_str());
    }
    ImGui::End();
}

void DrawHoldPanel(const Board& board, 
                   const ImVec2& SetNextWindowPosVector,
                   const ImVec2& SetNextWindowSizeVector) {
    ImGui::SetNextWindowPos(SetNextWindowPosVector);
    ImGui::SetNextWindowSize(SetNextWindowSizeVector);
    ImGui::Begin("Hold");
    PieceType held = board.GetHeldPieceType();
    if (held != PieceType::EMPTY) {
        Color c = GetColorForPiece(held);
        ImGui::ColorButton("##heldcolor",
            {c.r/255.f,c.g/255.f,c.b/255.f,1.f},
            ImGuiColorEditFlags_NoTooltip|ImGuiColorEditFlags_NoDragDrop,
            {30,30});
        ImGui::SameLine();
        ImGui::TextUnformatted(PieceTypeToString(held).c_str());
    } else {
        ImGui::Text("Empty");
    }
    ImGui::End();
}

void DrawHistoryPanel(const std::vector<std::string>& history, 
                      const ImVec2& SetNextWindowPosVector,
                      const ImVec2& ScrollRegionSizeVector) {
    ImGui::SetNextWindowPos(SetNextWindowPosVector);
    ImGui::Begin("Command History");
      ImGui::BeginChild("ScrollRegion", ScrollRegionSizeVector, true);
      for (const std::string& cmd : history)
        ImGui::TextUnformatted(cmd.c_str());
      ImGui::EndChild();
    ImGui::End();
}

void DrawBoardGrid(const Board& board,
                   int offsetX,
                   int offsetY,
                   int cellSize)
{
    std::vector<PieceType> state = board.GetRenderableState();
    for (int r = 0; r < VISIBLE_BOARD_HEIGHT; ++r) {
        for (int c = 0; c < BOARD_WIDTH; ++c) {
        PieceType pt = state[r*BOARD_WIDTH + c];
        Color col = GetColorForPiece(pt);
        DrawRectangle(
            offsetX + c*cellSize,
            offsetY + r*cellSize,
            cellSize-2, cellSize-2,
            col);
        }
    }
}

}
