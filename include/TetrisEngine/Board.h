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

/**
 * @brief Tetris game board controller.
 * 
 * Manages game state, piece movement, collision detection, and scoring.
 */
class Board {
    public:
        Board(std::mt19937& rng_ref);

        /// @name Game Flow
        /// @{
        /**
         * @brief Reset the board for a new game.
         * 
         * Clears grid, resets score, and spawns the first piece.
         */
        void Reset();
        /**
         * @brief Spawn a specific piece type.
         * @param type PieceType to spawn
         * @return true if piece spawned successfully, false if game over
         */
        bool SpawnNewPiece(PieceType type);

        /**
         * @brief Spawn a random piece using 7-bag RNG.
         * @return true if piece spawned, false on game over
         */
        bool SpawnRandomPiece();
        /// @}


        /// @name Player Actions
        /// @{
        /**
         * @brief Move active piece by delta.
         * @param delta_x Horizontal movement (+right, -left)
         * @param delta_y Vertical movement (+up, -down)
         * @return true if movement succeeded
         */
        bool MoveActivePiece(int delta_x, int delta_y);

        /**
         * @brief Rotate active piece with SRS kicks.
         * @param direction RotationDirection
         * @return true if rotation succeeded
         */
        bool RotateActivePiece(RotationDirection direction);

        /**
         * @brief Instantly drop piece and lock it. 
         */
        void HardDropActivePiece();
        /// @}

        /// @name Game State
        /// @{
        /**
         * @brief Check if the game has ended.
         * @return true if game over condition detected, false otherwise
         */
        bool IsGameOver() const { return isGameOverFlag; }

        /**
         * @brief Get piece type at board coordinate.
         * @param col Horizontal position (0 = leftmost)
         * @param row_from_bottom Vertical position (0 = bottom row)
         * @return PieceType at requested position
         * @warning Returns PieceType::EMPTY for invalid coordinates
         */
        PieceType GetCellState(int col, int row_from_bottom) const;

        /**
         * @brief Access active tetromino.
         * @return Raw pointer to current piece (nullptr if none active)
         */
        const Piece* GetCurrentPiece() const { return currentPiece.get(); }

        /**
         * @brief Get active piece's grid position.
         * @return Top-left coordinate of piece's 4x4 bounding box
         * @note Position uses bottom-row=0 coordinate system
         */
        Point GetCurrentPiecePosition() const { return currentPieceTopLeftPos; }

        /**
         * @brief Retrieve held piece type.
         * @return PieceType in hold slot, PieceType::EMPTY if none
         */
        PieceType GetHeldPieceType() const;

        /**
         * @brief Get current score.
         * @return Score value including bonuses
         */
        int GetScore() const { return score; }

        /**
         * @brief Get total cleared lines.
         * @return Cumulative lines cleared this game
         */
        int GetLinesCleared() const { return linesClearedTotal; }

        /**
         * @brief Get visible board state for rendering.
         * @return 1D array (row-major order) of PieceTypes
         * @note Includes active piece and locked cells
         * @details Rows ordered top-to-bottom (index 0 = top row)
         */
        std::vector<PieceType> GetRenderableState() const;

        /**
         * @brief Get upcoming pieces.
         * @return Next 5 pieces in queue. Represented as PieceType.
         */
        std::vector<PieceType> GetNextQueue() const;
        /// @}

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
        
        /// @name Display
        /// @{
        /**
         * @brief Render board state to screen.
         * @param screenWidth Width of rendering area in pixels
         * @param screenHeight Height of rendering area in pixels
         * @param show_hidden True to display hidden buffer rows (20-26)
         * @note Uses bottom-row=0 coordinate system for rendering
         */
        void PrintBoard(const int screenWidth, const int screenHeight, bool show_hidden) const;

        /**
         * @brief Get color mapping for piece types.
         * @param pt Piece type to map
         * @return Raylib Color value for visualization
         * @retval GRAY for EMPTY/invalid types
         */
        Color GetColorForPieceType(PieceType pt) const;
        /// @}

    private:
        std::array<PieceType, TOTAL_BOARD_HEIGHT * BOARD_WIDTH> grid;

        std::unique_ptr<Piece> currentPiece;
        Point currentPieceTopLeftPos;

        bool isGameOverFlag;
        int score;
        int linesClearedTotal;
    // Could add: level

    /// @name Internal Game Logic
    /// @{
    public:
        /**
         * @brief Places piece on grid, clears lines, and checks game over.
         * Based on the following scoring guidelines: https://tetris.fandom.com/wiki/Scoring
         */
        void LockActivePiece();

        /**
         * @brief Clears lines on the board
         * @return number of lines cleared in this step
         */
        int ClearFullLines();

        /**
         * @brief Sets all grid cells to PieceType::EMPTY
         */
        void InitializeGrid();

        /**
         * @brief figures out where a piece should spawn, using (x,y) coordinate system with bottom left as {0,0}
         * @param type PieceType of tetronimo
         * @return Point coordinate of where to spawn top left corner of piece
         */
        Point CalculateSpawnPosition(PieceType type);

        /**
         * @brief Checks if the piece (defined by its 4x4 representation) is valid at the given board top-left position.
         * @param uint16_t bit representation of tetronimo
         * @param top_left_pos Point coordinate of where to spawn top left corner of piece
         * @return True if position is valid. False otherwise
         */
        bool IsValidPosition(uint16_t piece_representation, Point top_left_pos) const;
    /// @}

    /// @name Piece Factory
    /// @{
        /**
         * @brief Given a PieceType, returns a unique_ptr containing an instance of said Piece
         * @param type tetris::PieceType of tetronimo
         * @return std::unique_ptr<Piece> instance
         */
        std::unique_ptr<Piece> CreatePieceByType(PieceType type);

    private:
        std::mt19937& rng; 
        std::vector<PieceType> grab_bag;
        std::vector<PieceType> grab_bag_next;
        size_t index;
        PieceType last_piece;
        bool last_piece_is_none;
    /// @}


    /// @name SRS Kick Data & Logic
    /// @{
    public:
        /**
         * @brief Returns a list of kick offsets to try for a given rotation.
         * @param type PieceType of tetronimo
         * @param from_rotation initial rotation state (STATE_0 | STATE_2 | STATE_R | STATE_L)
         * @param to_rotation final rotation state (STATE_0 | STATE_2 | STATE_R | STATE_L)
         * @return corresponding vector of Point coordinate offsets
         */
        const std::vector<Point>& GetSrsKickData(PieceType type, RotationState from_rotation, RotationState to_rotation) const;

        /**
         * @brief holds a piece, and disables further swaps until another piece has been dropped OR board is reset
         */
        void HoldPiece();

    private: 
        std::unique_ptr<Piece> held_piece;
        bool canHold;
    /// @}

    /// @name Spin Detection
    /// @{
    public: 
        /* Test for T-Spins
         * 0 = no T-Spin
         * 1 = T-Spin Mini
         * 2 = T-Spin
         * int T-Spin status code
         */
        int IsTSpin() const;

        /*
        * @brief Detects minispins for every piece except O.
        * @return true if minispin detected, false otherwise
        */
        bool IsAllMiniSpin() const;

    private:
        int back_to_back;
        mutable bool lastMoveWasRotation;
    /// @}

    // debugging only
    public:
        void PrintBoardText(bool show_hidden) const;
};

} // namespace tetris

#endif BOARD_H