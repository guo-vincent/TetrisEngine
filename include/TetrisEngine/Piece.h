#ifndef PIECE_H
#define PIECE_H

// See: https://isaiahgrace.github.io/tetris/board.html for how pieces are implemented

// All representation tables are in ROW_MAJOR format
/* e.g.
0 1 2 3
4 5 6 7
8 9 a b
c d e f

Indices 3, 7, b, c, d, e, f are omitted and assumed to be set to 0 
if the piece can be represented in a 3 by 3 square
*/


#include <algorithm>
#include <iostream>
#include <string>
#include <array>
#include <cstdint>
#include <utility>

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
        uint16_t representation; // 4 by 4 bitboard
        bool placed;             // Can we still move it? Or is it locked?

        virtual const uint16_t* get_representation_table() const = 0;

        piece() : piece_type(PieceType::Empty), rotation(0), representation(0), placed(false) {}
        virtual ~piece() = default;

        void print_piece();

        void flip_piece_clockwise();
        void flip_piece_counterwise();
    };

    class NULL_piece : public piece {
        public:
            NULL_piece();
    };

    class I_piece : public piece {
        public:
            I_piece();

        private: 
            const uint16_t representation_table[4] = {
                0x0F00, // 0000 1111 0000 0000 (horizontal)
                0x2222, // 0010 0010 0010 0010 (vertical)
                0x00F0, // 0000 0000 1111 0000 (horizontal, offset down)
                0x4444  // 0100 0100 0100 0100 (vertical, offset right)
            };

            const uint16_t* get_representation_table() const override { 
                return representation_table; 
            }
    };

    class J_piece : public piece {
        public:
            J_piece();

        private:
            // Note: we should only use first 9 bits, but this represents 4 by 4
            const uint16_t representation_table[4] = {
                0x8E00, // Rotation 0: 1000 1110 0000 0000 (X... XXX.)
                0x44C0, // Rotation 1: 0100 0100 1100 0000 (.X.. .X.. XX..)
                0x0E20, // Rotation 2: 0000 1110 0010 0000 (.... XXX. ..X.)
                0x6440  // Rotation 3: 0110 0100 0100 0000 (.XX. .X.. .X..)
            };

            const uint16_t* get_representation_table() const override { 
                return representation_table; 
            }
    };

    class L_piece : public piece {
        public:
            L_piece();

        private:
            const uint16_t representation_table[4] = {
                0x2E00, // Rotation 0: 0010 1110 0000 0000 (..X. XXX.)
                0x4460, // Rotation 1: 0100 0100 0110 0000 (.X.. .X.. .XX.)
                0x0E80, // Rotation 2: 0000 1110 1000 0000 (.... XXX. X...)
                0xC440  // Rotation 3: 1100 0100 0100 0000 (XX.. .X.. .X..)
            };

            const uint16_t* get_representation_table() const override { 
                return representation_table; 
            }
    };

    class O_piece : public piece {
        public:
            O_piece();

        private:
            const uint16_t representation_table[4] = { // can't actually rotate
                0x0660, // Rotation 0: 0110 0110 0000 0000 (.XX. .XX.)
                0x0660, // Rotation 1: 0110 0110 0000 0000 (.XX. .XX.)
                0x0660, // Rotation 2: 0110 0110 0000 0000 (.XX. .XX.)
                0x0660  // Rotation 3: 0110 0110 0000 0000 (.XX. .XX.)
            };

            const uint16_t* get_representation_table() const override { 
                return representation_table; 
            }
    };

    class S_piece : public piece {
        public:
            S_piece();
        
        private:
            const uint16_t representation_table[4] = {
                0x6C00, // Rotation 0: 0110 1100 0000 0000 (.XX. XX..)
                0x8C40, // Rotation 1: 1000 1100 0100 0000 (X... XX.. .X..)
                0x6C00, // Rotation 2: 0000 0110 1100 0000 (.... .XX. XX..)
                0x4620  // Rotation 3: 0100 0110 0010 0000 (.X.. .XX. ..X.)
            };

            const uint16_t* get_representation_table() const override { 
                return representation_table; 
            }
    };

    class T_piece : public piece {
        public:
            T_piece();

        private:
            const uint16_t representation_table[4] = {
                0x4E00, // Rotation 0: 0100 1110 0000 0000 (.X.. XXX.)
                0x4640, // Rotation 1: 0100 0110 0100 0000 (.X.. .XX. .X..)
                0x0E40, // Rotation 2: 0000 1110 0100 0000 (.... XXX. .X..)
                0x4C40  // Rotation 3: 0100 1100 0100 0000 (.X.. XX.. .X..)
            };

            const uint16_t* get_representation_table() const override { 
                return representation_table; 
            }
    };

    class Z_piece : public piece {
        public:
            Z_piece();

        private:
            const uint16_t representation_table[4] = {
                0xC600, // Rotation 0: 1100 0110 0000 0000 (XX.. .XX.)
                0x2640, // Rotation 1: 0010 0110 0100 0000 (..X. .XX. .X..)
                0x0630, // Rotation 2: 0000 0110 0011 0000 (.... .XX. ..XX)
                0x4C80  // Rotation 3: 0100 1100 1000 0000 (.X.. XX.. X...)
            };

            const uint16_t* get_representation_table() const override { 
                return representation_table; 
            }
    };
}

#endif PIECE_H