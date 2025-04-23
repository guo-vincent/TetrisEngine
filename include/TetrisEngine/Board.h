#ifndef BOARD_H
#define BOARD_H

// Note: Tetr.io tetris boards are 10 columns wide and 20 rows tall
// Using the following bitboard implementation: https://isaiahgrace.github.io/tetris/board.html
// 3-bits per block occupied = 8 total blocks = all combinations covered
// Top left corner is index 0, bottom right is final index
// Note that the first 6 rows are actually hidden, while our game is really played at row 6 and onwards

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
            size_t convert_coord(const uint16_t) const;
            void insert(const tetris::PieceType);

            // Board state
            bool is_valid() const;

            bool is_valid_move() const;
            bool is_loss() const;
            void print_board() const;

            uint16_t move_down();
            uint16_t move_left();
            uint16_t move_right();
            uint16_t drop();
            
            struct iterator {
                iterator();
                iterator(const size_t);

                // Increment/Decrement operations
                iterator& operator++();
                iterator operator++(int);
                iterator& operator--();
                iterator operator--(int);

                // Comparison operations
                bool operator==(const iterator &) const;
                bool operator!=(const iterator &) const;

                // Access operations
                iterator begin();
                iterator row_begin(size_t);
                iterator col_begin(size_t);
                iterator end();
                iterator row_end(size_t);
                iterator col_end(size_t);
                uint32_t operator[](size_t);

                size_t index;
                bool value;
            };

            size_t pieces;
            size_t squares_filled;
            size_t rows;
            size_t cols;

        private: 
            // 27 row by 10 col bit array 
            std::bitset<810> bits;
    };
}

#endif BOARD_H