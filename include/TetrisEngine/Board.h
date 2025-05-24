#ifndef BOARD_H
#define BOARD_H

// Note: Tetr.io tetris boards are 10 columns wide and 20 rows tall
// Using the following bitboard implementation: https://isaiahgrace.github.io/tetris/board.html
// 3-bits per block occupied = 8 total blocks = all combinations covered
// The board has 27 rows, with row 0 being the bottom (where the pieces stack) and row 26 being the top.
// The visible area is rows 0-19 (20 rows), and rows 20-26 are hidden (7 rows).
// Bit boards are in row major format also.

/* Board setup
Rows:
    26
    25
    ...
    2
    1
    0
Cols: 0 1 2 3 4 5 6 7 8 9
*/

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <array>
#include <cstdint>
#include <cstddef>
#include <bitset>

#include "Piece.h"

// TODO: implement Zobrist Hash Class & change == and != overloads
// TODO: implement rest of the operators, including iterators in Board.cpp
// TODO: write unit tests in tests\test_board.cpp
// TODO: implement pseudolegal and legal move generator class

namespace tetris {
    class board {        
        public:
            board();

            // Board comparison
            bool operator==(const board &) const;
            bool operator!=(const board &) const;

            // This is for generating a piece onto the board. May require more parameters
            // Caution: the visible area for the tetris board is only 20 blocks high + 1 which can be seen but is not gridded
            // So we should spawn the block within that area. 
            void insert_new_piece(const tetris::PieceType);

            // Checks if a board is a loss
            bool is_loss() const;

            // Checks if a board state is valid, or if the board needs to be updated (e.g. full row)
            bool needs_rows_cleared() const;

            // Checks if a move is valid. May require more parameters
            bool is_valid_move() const;

            // Given an invalid board, makes adjustments to make the board valid
            void make_rows_valid();

            // prints the state of the board. Currently we can just print '.' and numbers for now
            // but this will later be replaced with actual graphics
            void print_board() const;

            // These tell the board to move the current piece in hand in the given direction
            // It may make more sense to move these into the piece class, which already has some logic
            // on rotations
            uint16_t move_down();
            uint16_t move_left();
            uint16_t move_right();
            uint16_t drop();

            size_t rows;
            size_t cols;

            struct iterator {
                const board* parent;
                size_t       index;

                // Constructors
                iterator(const board* b = nullptr, size_t idx = 0)
                : parent(b), index(idx) {}

                // Increment/Decrement
                iterator& operator++()    { ++index; return *this; }
                iterator  operator++(int) { iterator tmp = *this; ++index; return tmp; }
                iterator& operator--()    { --index; return *this; }
                iterator  operator--(int) { iterator tmp = *this; --index; return tmp; }

                // Comparison
                bool operator==(iterator const& other) const { return index == other.index && parent == other.parent; }
                bool operator!=(iterator const& other) const { return !(*this == other); }

                // Dereference: pull 3 bits out of parent->bits
                uint8_t operator*() const {
                    assert(parent);
                    auto const& bits = parent->bits;
                    const size_t bit_pos = index * 3;
                    return  uint8_t(
                        (bits[bit_pos+2] << 2) |
                        (bits[bit_pos+1] << 1) |
                        bits[bit_pos]
                    );
                }

                // Row iterators if you still need them
                iterator row_begin(size_t row) const { return iterator(parent, row * parent->cols); }
                iterator row_end  (size_t row) const { return iterator(parent, (row+1) * parent->cols); }
            };

            // Provide proper begin()/end() on the board itself
            iterator begin()       { return iterator(this, 0); }
            iterator end()         { return iterator(this, rows*cols); }

            // Const overloads too, if you want
            iterator begin() const { return iterator(this, 0); }
            iterator end()   const { return iterator(this, rows*cols); }

        private: 
            // 27 row by 10 col block array. Each block is represented by 3 bits to represent different piece types.
            std::bitset<810> bits;

            // Track rows modified by last insert for optimization of eliminating rows
            std::vector<size_t> last_placed_rows; 

    };

    // Creates a zobrist hash for the tetris board, used for quickly checking if two board positions are equal
    // Useful for testing
    struct zobrist {
        
    };
};

#endif BOARD_H