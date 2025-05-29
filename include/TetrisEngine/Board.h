#ifndef BOARD_H
#define BOARD_H

// Note: Tetr.io tetris boards are 10 columns wide and 20 rows tall
// The board has 27 rows, with row 0 being the bottom (where the pieces stack) and row 26 being the top.
// We do this to make keeping track of extra blocks easier.
// The visible area is rows 0-19 (20 rows), and rows 20-26 are hidden (7 rows).
// Bit boards are in row major format also.

/* Board setup
Rows (y):
    26
    25
    ...
    2
    1
    0
Cols: 0 1 2 3 4 5 6 7 8 9 (x)
*/

#include "Piece.h"
#include <vector>
#include <array>
#include <memory> 
#include <bitset>
#include <functional>
#include <random>

namespace tetris {

// Board dimensions and constants
constexpr int BOARD_WIDTH = 10;
constexpr int VISIBLE_BOARD_HEIGHT = 20; // Rows 0 to 19 from bottom are visible
constexpr int TOTAL_BOARD_HEIGHT = 27;   // Rows 0 to 26 from bottom. Rows 20-26 are buffer/spawn area.

// Helper to define rotation transitions for SRS kicks
enum class RotationDirection {
    CLOCKWISE,
    COUNTER_CLOCKWISE,
    ONE_EIGHTY
};

class Board {
public:
    Board();

    // Game Flow
    void Reset();                       // Resets the board for a new game
    bool SpawnNewPiece(PieceType type); // Spawns a specific piece, returns false if spawn fails (game over)
    bool SpawnRandomPiece();            // Spawns a random piece

    // Player Actions
    bool MoveActivePiece(int delta_x, int delta_y);         // Tries to move, returns true on success
    bool RotateActivePiece(RotationDirection direction);    // Tries to rotate, returns true on success
    void HardDropActivePiece();

    // Game State & Info
    bool IsGameOver() const { return isGameOverFlag; }
    PieceType GetCellState(int col, int row_from_bottom) const;              // For querying grid
    const Piece* GetCurrentPiece() const { return currentPiece.get(); }
    Point GetCurrentPiecePosition() const { return currentPieceTopLeftPos; } // Top-left of 4x4 box
    PieceType GetHeldPieceType() const;
    int GetScore() const { return score; }
    int GetLinesCleared() const { return linesClearedTotal; }
    std::vector<PieceType> GetRenderableState() const;
    std::vector<PieceType> GetNextQueue() const;

    // Iterator for board cells
    class CellIterator {
        public:
            CellIterator(const Board* board_ptr, int c, int r) : board(board_ptr), col(c), row(r) {}

            PieceType operator*() const { return board->GetCellState(col, row); }
            CellIterator& operator++() { // Simple row-major increment
                col++;
                if (col >= BOARD_WIDTH) {
                    col = 0;
                    row--; // Iterating from top-visible down to bottom for typical display
                }
                return *this;
            }
            bool operator!=(const CellIterator& other) const {
                return col != other.col || row != other.row || board != other.board;
            }

        private:
            const Board* board;
            int col;
            int row;
        };

    CellIterator visible_begin() const { return CellIterator(this, 0, VISIBLE_BOARD_HEIGHT - 1); }
    CellIterator visible_end() const { return CellIterator(this, 0, -1); }
    
    // Display
    void PrintBoard(const int screenWidth, const int screenHeight, bool show_hidden) const;
    Color GetColorForPieceType(PieceType pt) const;

    // Grid stores PieceType for each cell. Row 0 is bottom.
    std::array<PieceType, TOTAL_BOARD_HEIGHT * BOARD_WIDTH> grid;

    std::unique_ptr<Piece> currentPiece;
    Point currentPieceTopLeftPos; // (column, row_from_bottom) for the top-left of piece's 4x4 box

    bool isGameOverFlag;
    int score;
    int linesClearedTotal;
    // Could add: level

    // Internal Game Logic
    void LockActivePiece(); // Places piece on grid, clears lines, checks game over
    int ClearFullLines();   // Returns number of lines cleared in this step
    void InitializeGrid();  // Sets all grid cells to PieceType::EMPTY
    Point CalculateSpawnPosition(PieceType type);

    // Collision and Movement Validation
    // Checks if the piece (defined by its 4x4 representation) is valid at the given board top-left position.
    bool IsValidPosition(uint16_t piece_representation, Point top_left_pos) const;

    // Piece Factory
    std::unique_ptr<Piece> CreatePieceByType(PieceType type);
    std::mt19937 rng;
    std::vector<PieceType> grab_bag;
    std::vector<PieceType> grab_bag_next;
    size_t index;
    PieceType last_piece;
    bool last_piece_is_none;


    // SRS Kick Data and Logic
    // Returns a list of kick offsets to try for a given rotation.
    const std::vector<Point>& GetSrsKickData(PieceType type, RotationState from_rotation, RotationState to_rotation) const;

    // Hold logic
    void HoldPiece();
    private: 
        std::unique_ptr<Piece> held_piece;
        bool canHold;

    // debugging only
    void PrintBoardText(bool show_hidden) const;
};

} // namespace tetris

#endif BOARD_H