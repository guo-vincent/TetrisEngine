#ifndef Ui_H
#define Ui_H

#include "Game.h"
#include <vector>
#include <string>

#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>

namespace tetris {

// Wrapper
bool DrawPlayer(Game& game, int playerNum, int offsetX, int offsetY, std::vector<std::string>& commandHistory, bool gameOver, int cellSize = 30);

// Controls
bool DrawControlsPanel(Board& board, std::vector<std::string>& commandHistory, bool gameOver, const ImVec2& SetNextWindowPosVector = ImVec2(600,100));

// Next Queue
void DrawQueuePanel(const Board& board, const ImVec2& SetNextWindowPosVector = ImVec2(450,100), const ImVec2& SetNextWindowSizeVector = ImVec2(100,0));

// Hold
void DrawHoldPanel(const Board& board, const ImVec2& SetNextWindowPosVector = ImVec2(450,350), const ImVec2& SetNextWindowSizeVector = ImVec2(100,0));

// Command History
void DrawHistoryPanel(const std::vector<std::string>& commandHistory, const ImVec2& SetNextWindowPosVector = ImVec2(600,350), const ImVec2& ScrollRegionSizeVector = ImVec2(250,200));

// Block Grid
void DrawBoardGrid(const Board& board, int offsetX, int offsetY, int cellSize = 30);

} // namespace tetris::ui

#endif Ui_H