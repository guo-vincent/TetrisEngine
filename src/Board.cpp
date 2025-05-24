#include "../include/TetrisEngine/Board.h"
#include "../include/TetrisEngine/Piece.h"

#include <memory>
#include <vector>
#include <stdexcept>
#include <set>

namespace tetris {
    board::board() {
        rows = 27;
        cols = 10;
    }


    void board::insert_new_piece(const PieceType type) {
        std::set<size_t> affected_rows;
        // Initalize piece. Unique_ptrs used to simplify memory management
        std::unique_ptr<piece> p;
        switch (type) {
            case PieceType::I: p = std::make_unique<I_piece>(); break;
            case PieceType::J: p = std::make_unique<J_piece>(); break;
            case PieceType::L: p = std::make_unique<L_piece>(); break;
            case PieceType::O: p = std::make_unique<O_piece>(); break;
            case PieceType::S: p = std::make_unique<S_piece>(); break;
            case PieceType::T: p = std::make_unique<T_piece>(); break;
            case PieceType::Z: p = std::make_unique<Z_piece>(); break;
            default: throw std::invalid_argument("Invalid piece type");
        }

        // Extract occupied blocks from the 4x4 bitboard
        std::vector<std::pair<int, int>> blocks;
        uint16_t rep = p->representation;
        for (int i = 0; i < 16; ++i) {
            if (rep & (1 << (15 - i))) {
                int block_row = i / 4;  // 0-3 (top to bottom in 4x4 grid)
                int block_col = i % 4;  // 0-3 (left to right)
                blocks.emplace_back(block_row, block_col);
            }
        }

        // Find min/max columns and rows in the piece's grid
        int min_col = 3, max_col = 0;
        int min_row = 3, max_row = 0;
        for (const auto& [r, c] : blocks) {
            min_col = std::min(min_col, c);
            max_col = std::max(max_col, c);
            min_row = std::min(min_row, r);
            max_row = std::max(max_row, r);
        }
        int width = max_col - min_col + 1;
        
        // Calculate spawn position (x, y)
        int x = (10 - width) / 2 + min_col;  // Center horizontally
        int y = 20 - min_row;  // Align lowest block to hidden row 20

        // Check if all blocks fit and are empty
        bool can_place = true;
        for (const auto& [r, c] : blocks) {
            int board_col = x + c;
            int board_row = y + r;
            
            // Check if block is empty (3 bits per block)
            size_t bit_pos = (board_row * 10 + board_col) * 3;
            uint8_t val = (bits.test(bit_pos) ? 1 : 0) << 2 |
                          (bits.test(bit_pos + 1) ? 1 : 0) << 1 |
                          (bits.test(bit_pos + 2) ? 1 : 0);
            if (val != static_cast<uint8_t>(PieceType::Empty)) {
                can_place = false;
                break;
            }

            affected_rows.insert(static_cast<size_t>(board_row));
        }

        // You tried generating a new piece, but one of the tiles was blocked, so player automatically loses.
        if (!can_place) {
            // set a loss flag here!
            return;
        }

        last_placed_rows.assign(affected_rows.begin(), affected_rows.end());

        // Place the piece on the board
        for (const auto& [r, c] : blocks) {
            int board_col = x + c;
            int board_row = y + r;
            size_t bit_pos = (board_row * 10 + board_col) * 3;
            uint8_t piece_val = static_cast<uint8_t>(type);
            
            // Set the 3 bits for this block
            bits.set(bit_pos, (piece_val >> 2) & 1);
            bits.set(bit_pos + 1, (piece_val >> 1) & 1);
            bits.set(bit_pos + 2, piece_val & 1);
        }
    }

    bool board::needs_rows_cleared() const {
        // Check only the rows touched by the last placed piece
        for (const size_t row : last_placed_rows) {
            bool row_full = true;

            // Check all columns in this row
            for (size_t col = 0; col < cols; ++col) {
                const size_t bit_pos = (row * cols + col) * 3;
                const uint8_t block_val = 
                    (bits.test(bit_pos) << 2) |
                    (bits.test(bit_pos + 1) << 1) |
                    bits.test(bit_pos + 2);

                if (block_val == static_cast<uint8_t>(PieceType::Empty)) {
                    row_full = false;
                    break;
                }
            }

            if (row_full) return false; // Needs row clearing
        }

        return true; // No full rows detected
    }  

    // This should only be called after the piece is locked (e.g. after drop())
    bool board::is_loss() const {
        // Check if any rows above the visible area are occupied
        for (const size_t row : last_placed_rows) {
            if (row > 19) { 
                // This is a loss if the piece is locked in
                return true;
            }
        }
        return false;
    }
}