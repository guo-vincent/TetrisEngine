#ifndef Ui_H
#define Ui_H

#include "Game.h"
#include <vector>
#include <string>

#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>

namespace tetris {

// Draw the “Controls” window, mutate game and history as needed.
// Returns true if gameOver flipped from false to true.
// Defaults: 
/*
*/
bool DrawControlsPanel(Game& game, std::vector<std::string>& commandHistory, bool gameOver, const ImVec2& SetNextWindowPosVector = ImVec2(600,100));

// Draw the “Next Queue” window
void DrawQueuePanel(const Board& board, const ImVec2& SetNextWindowPosVector = ImVec2(450,100), const ImVec2& SetNextWindowSizeVector = ImVec2(100,0));

// Draw the “Hold” window
void DrawHoldPanel(const Board& board, const ImVec2& SetNextWindowPosVector = ImVec2(450,350), const ImVec2& SetNextWindowSizeVector = ImVec2(100,0));

// Draw the “Command History” window
void DrawHistoryPanel(const std::vector<std::string>& commandHistory, const ImVec2& SetNextWindowPosVector = ImVec2(600,350), const ImVec2& ScrollRegionSizeVector = ImVec2(250,200));

// Draw the block grid of the board
void DrawBoardGrid(const Board& board, int offsetX, int offsetY, int cellSize);

} // namespace tetris::ui

#endif Ui_H
