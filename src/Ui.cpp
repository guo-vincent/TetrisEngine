// UI.cpp
#include "TetrisEngine/Ui.h"
#include "TetrisEngine/Board.h"
#include "TetrisEngine/Game.h"
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
        case PieceType::G: return "G";
        case PieceType::EMPTY: return "E";
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
        {PieceType::G, LIGHTGRAY}
    };
    return colorMap.count(pt) ? colorMap[pt] : RAYWHITE;
}

bool DrawControlsPanel(Board& board,
                       int playerNum,
                       std::vector<std::string>& commandHistory,
                       bool gameOver,
                       const ImVec2& SetNextWindowPosVector
                    )
{
    ImGui::SetNextWindowPos(SetNextWindowPosVector);

    std::string title = "Controls##" + std::to_string(playerNum);
    ImGui::Begin(title.c_str());
    
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

        static int garbage_lines = 0;
        ImGui::InputInt("Garbage", &garbage_lines);

        if (ImGui::Button("Add Garbage")) {
            board.AddGarbageToQueue(garbage_lines);
            commandHistory.push_back("Added garbage lines to queue");
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

    return gameOver;
}

void DrawQueuePanel(const Board& board,
                    int playerNum,
                    const ImVec2& SetNextWindowPosVector,
                    const ImVec2& SetNextWindowSizeVector) {
    ImGui::SetNextWindowPos(SetNextWindowPosVector);
    ImGui::SetNextWindowSize(SetNextWindowSizeVector);
    std::string title = "Queue##" + std::to_string(playerNum);
    ImGui::Begin(title.c_str());
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
                   int playerNum,
                   const ImVec2& SetNextWindowPosVector,
                   const ImVec2& SetNextWindowSizeVector) {
    ImGui::SetNextWindowPos(SetNextWindowPosVector);
    ImGui::SetNextWindowSize(SetNextWindowSizeVector);
    std::string title = "Hold##" + std::to_string(playerNum);
    ImGui::Begin(title.c_str());
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
                      int playerNum,
                      const ImVec2& SetNextWindowPosVector,
                      const ImVec2& ScrollRegionSizeVector) {
    ImGui::SetNextWindowPos(SetNextWindowPosVector);
    std::string title = "Command History##" + std::to_string(playerNum);
    ImGui::Begin(title.c_str());
      ImGui::BeginChild("ScrollRegion", ScrollRegionSizeVector, true);
      for (const std::string& cmd : history)
        ImGui::TextUnformatted(cmd.c_str());
      ImGui::EndChild();
    ImGui::End();
}

void DrawGarbagePanel(const Board& board, 
                   int playerNum,
                   const ImVec2& SetNextWindowPosVector,
                   const ImVec2& SetNextWindowSizeVector) {
    ImGui::SetNextWindowPos(SetNextWindowPosVector);
    ImGui::SetNextWindowSize(SetNextWindowSizeVector);
    std::string title = "Garbage Stats##" + std::to_string(playerNum);
    ImGui::Begin(title.c_str());
    std::string b2b = "Back to Back: " + std::to_string(board.GetB2BChain());
    std::string combo = "Combo: " + std::to_string(board.GetCombo());
    std::string garbage = "Garbage in Queue: " + std::to_string(board.GetGarbageQueue());
    ImGui::TextUnformatted(b2b.c_str());
    ImGui::TextUnformatted(combo.c_str());
    ImGui::TextUnformatted(garbage.c_str());
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

bool DrawPlayer(Game& game,
                int playerNum, 
                int offsetX, 
                int offsetY,  
                std::vector<std::string>& commandHistory, 
                bool gameOver,
                int cellSize){
    ImGui::PushID(playerNum);

    gameOver = DrawControlsPanel(game.getBoard(playerNum), playerNum, commandHistory, gameOver, ImVec2((float)offsetX + 500, (float)offsetY));

    if (IsKeyPressed(KEY_T)) {
        game.Reset();
    }

    DrawQueuePanel(game.getBoard(playerNum), playerNum, ImVec2(static_cast<float>(offsetX + 350), static_cast<float>(offsetY)));
    DrawHoldPanel(game.getBoard(playerNum), playerNum, ImVec2(static_cast<float>(offsetX + 350), static_cast<float>(offsetY + 300)));
    DrawHistoryPanel(commandHistory, playerNum, ImVec2(static_cast<float>(offsetX + 500), static_cast<float>(offsetY + 300)));
    DrawGarbagePanel(game.getBoard(playerNum), playerNum, ImVec2(static_cast<float>(offsetX), static_cast<float>(offsetY + 650)));

    ImGui::PopID();

    // draw the actual tetris board
    DrawBoardGrid(game.getBoard(playerNum), offsetX, offsetY, cellSize);

    return gameOver;
}

}
