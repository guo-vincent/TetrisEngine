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

#include <array>
#include <cstdint>
#include <raylib.h>

namespace tetris {

    // Represents a 2D point or offset (integer-based for board coordinates and kicks)
    struct Point {
        int x = 0;
        int y = 0;

        Point operator+(const Point& other) const {
            return {x + other.x, y + other.y};
        }
        bool operator==(const Point& other) const {
            return x == other.x && y == other.y;
        }
    };

    enum class PieceType : uint8_t {
        EMPTY = 0,
        I = 1,
        J = 2,
        L = 3,
        O = 4,
        S = 5,
        T = 6,
        Z = 7
    };

    enum class RotationState : uint8_t {
        STATE_0 = 0, // Spawn state
        STATE_R = 1, // Clockwise rotation from spawn (or 1st CWR)
        STATE_2 = 2, // Two rotations from spawn (or 2nd CWR)
        STATE_L = 3  // Counter-clockwise rotation from spawn (or 3rd CWR)
    };

    // Abstract base class for all Tetris pieces
    class Piece {
        public:
            Piece(PieceType piece_type): type(piece_type), currentRotation(RotationState::STATE_0), color(WHITE){}
            virtual ~Piece() = default;

            PieceType GetType() const { return type; }
            RotationState GetCurrentRotation() const { return currentRotation; }
            void SetCurrentRotation(RotationState new_rotation) { currentRotation = new_rotation; }

            // Returns the 4x4 bitmask representation for a given rotation state.
            // Bits are typically row-major: 0-3 (top row), 4-7, 8-11, 12-15 (bottom row).
            // e.g., 0x0F00 represents a horizontal I-piece in the second row of the 4x4 grid.
            virtual uint16_t GetRepresentation(RotationState state) const = 0;

            // Helper to get the representation for the current rotation state
            uint16_t GetCurrentRepresentation() const { return GetRepresentation(currentRotation); }
            Color GetColor() const { return color; }

        protected:
            PieceType type;
            RotationState currentRotation;
            Color color;
        };

    // Concrete piece classes

    // NOTE: ALL PIECES ARE MIRRORED ABOUT THE X-AXIS DUE TO ROW 0 BEING ON THE BOTTOM

    /*
    Visual representation Format

    0   R   2   L
    */

    class PieceI : public Piece {
        public:
            PieceI() : Piece(PieceType::I) {}
            uint16_t GetRepresentation(RotationState state) const override {
                static const std::array<uint16_t, 4> representations = {
                    0x00F0, // STATE_0 
                    0x2222, // STATE_R 
                    0x0F00, // STATE_2 
                    0x4444  // STATE_L 
                };
                return representations[static_cast<uint8_t>(state)];
            }
        };
        /*
        .... ..X. .... .X.. 
        .... ..X. XXXX .X.. 
        XXXX ..X. .... .X.. 
        .... ..X. .... .X.. 
        */

    class PieceJ : public Piece {
        public:
            PieceJ() : Piece(PieceType::J) {}
            uint16_t GetRepresentation(RotationState state) const override {
                static const std::array<uint16_t, 4> representations = {
                    0x0E80, // STATE_0
                    0x4460, // STATE_R
                    0x2E00, // STATE_2
                    0xC440  // STATE_L
                };
                
                return representations[static_cast<uint8_t>(state)];
            }
        };
        /*
        ... .X. ..X XX. 
        XXX .X. XXX .X. 
        X.. .XX ... .X. 
        */

    class PieceL : public Piece {
        public:
            PieceL() : Piece(PieceType::L) {}
            uint16_t GetRepresentation(RotationState state) const override {
                static const std::array<uint16_t, 4> representations = {
                    0x0E20, // STATE_0
                    0x6440, // STATE_R
                    0x8E00, // STATE_2
                    0x44C0  // STATE_L
                };
                return representations[static_cast<uint8_t>(state)];
            }
        };
        /*
        ... .XX X.. .X. 
        XXX .X. XXX .X. 
        ..X .X. ... XX. 
        */

    class PieceO : public Piece {
        public:
            PieceO() : Piece(PieceType::O) {}
            uint16_t GetRepresentation(RotationState state) const override {
                static const std::array<uint16_t, 4> representations = {
                    0x0660, // .XX., .XX.
                    0x0660,
                    0x0660,
                    0x0660
                };
                return representations[static_cast<uint8_t>(state)];
            }
        };
        /*
        .... 
        .XX. 
        .XX. Repeat all states 
        ....
        */

    class PieceS : public Piece {
        public:
            PieceS() : Piece(PieceType::S) {}
            uint16_t GetRepresentation(RotationState state) const override {
                static const std::array<uint16_t, 4> representations = {
                    0x0C60, // STATE_0
                    0x2640, // STATE_R
                    0xC600, // STATE_2
                    0x4C80  // STATE_L
                };
                return representations[static_cast<uint8_t>(state)];
            }
        };
        /*
        ... ..X XX. .X.
        XX. .XX .XX XX.
        .XX .X. ... X..
        */

    class PieceT : public Piece {
        public:
            PieceT() : Piece(PieceType::T) {}
            uint16_t GetRepresentation(RotationState state) const override {
                static const std::array<uint16_t, 4> representations = {
                    0x0E40, // STATE_0
                    0x4640, // STATE_L
                    0x4E00, // STATE_2
                    0x4C40  // STATE_R
                };
                return representations[static_cast<uint8_t>(state)];
            }
        };
        /*
        ... .X. .X. .X. 
        XXX .XX XXX XX. 
        .X. .X. ... .X. 
        */

    class PieceZ : public Piece {
        public:
            PieceZ() : Piece(PieceType::Z) {}
            uint16_t GetRepresentation(RotationState state) const override {
                static const std::array<uint16_t, 4> representations = {
                    0x06C0, // STATE_0
                    0x4620, // STATE_R
                    0x6C00, // STATE_2
                    0x8C40  // STATE_L
                };
                return representations[static_cast<uint8_t>(state)];
            }

            /*
            ... .X. .XX X..
            .XX .XX XX. XX.
            XX. ..X ... .X.
            */
        };
}

#endif PIECE_H