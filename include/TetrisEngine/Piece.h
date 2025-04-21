#ifndef PIECE_H
#define PIECE_H

// See: https://isaiahgrace.github.io/tetris/board.html for how pieces are implemented

#include <algorithm>
#include <iostream>
#include <string>
#include <array>
#include <cstdint>

namespace tetris {
    enum struct PieceType {
        Empty = 0, 
        I = 1, 
        J = 2, 
        L = 3, 
        O = 4, 
        S = 5, 
        T = 6, 
        Z = 7
    };

    struct piece {
        PieceType piece_type;
        uint8_t rotation;

        // 4 by 4 bitboard
        uint16_t representation;

        // Can we still move it? Or is it locked?
        bool placed;

        const uint16_t representation_table[4] = {0, 0, 0, 0};

        piece();

        void print_piece();

        // Movement based functions. These are meant to be inherited
        void flip_piece_clockwise();
        void flip_piece_counterwise();
    };

    class NULL_piece : piece {
        public:
            NULL_piece();
    };

    class I_piece : piece {
        public:
            I_piece();

        private: 
            const uint16_t representation_table[4] = {
                0b0000111100000000u,
                0b0010001000100010u,
                0b0000000011110000u,
                0b0100010001000100u
            };
    };

    class J_piece : piece {
        public:
            J_piece();

        private:
            // Note: we should only use first 9 bits, but this represents 4 by 4
            const uint16_t representation_table[4] = {
                0b1000111000000000u,
                0b0110010001000000u,
                0b0000111000100000u,
                0b0100010011000000u
            };
    };

    class L_piece : piece {
        public:
            L_piece();

        private:
            const uint16_t representation_table[4] = {
                0b0010111000000000u,
                0b0100010001100000u,
                0b0000111010000000u,
                0b1100010001000000u
            };
    };

    class O_piece : piece {
        public:
            O_piece();

        private:
            const uint16_t representation_table[4] = {
                0b0000011001100000u,
                0b0000011001100000u,
                0b0000011001100000u,
                0b0000011001100000u
            };
    };

    class S_piece : piece {
        public:
            S_piece();
        
        private:
            const uint16_t representation_table[4] = {
                0b0110110000000000u,
                0b0100011000100000u,
                0b0000011011000000u,
                0b1000110001000000u
            };
    };

    class T_piece : piece {
        public:
            T_piece();

        private:
            const uint16_t representation_table[4] = {
                0b0100111000000000u,
                0b0100011001000000u,
                0b0000111001000000u,
                0b0100110001000000u
            };
    };

    class Z_piece : piece {
        public:
            Z_piece();

        private:
            const uint16_t representation_table[4] = {
                0b1100011000000000u,
                0b0010011001000000u,
                0b0000110001100000u,
                0b0100110010000000u
            };
    };
}

#endif PIECE_H