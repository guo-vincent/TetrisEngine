#include "../include/TetrisEngine/Board.h"
#include "../include/TetrisEngine/Piece.h"
namespace tetris {
    board::board() {
        pieces = 0;
        squares_filled = 0;
        rows = 27;
        cols = 10;
    }

    // Checks if two boards are the same
    bool board::board::operator==(const board &other) const {
        // Check if board states are the same
        if (!(  (this->pieces == other.pieces) || 
                (this->squares_filled == other.squares_filled) || 
                (this->rows == other.rows) ||
                (this->cols == other.cols)
             )
            ) {
                return false;
            }

        // Check if every bit individually is the same
        for (int i = 0; i < bits.size(); i++) {
            if (this->bits[i] != other.bits[i]) {
                return false;
            }
        }
        return true;
    }

    // Checks if two boards are not the same
    bool board::board::operator!=(const board &other) const {
        return !(*this == other);
    }

}