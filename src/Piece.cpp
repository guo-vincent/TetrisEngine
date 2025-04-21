#include "../include/TetrisEngine/Piece.h"
#include <cmath>

namespace tetris {
    using std::cout;
    using std::endl;

    piece::piece() {
        piece_type = PieceType::Empty;
        rotation = NULL;
        representation = 0;
        placed = NULL;
    }

    void piece::print_piece() {
        for (size_t i = 15; i >= 0; i--) {
            cout << ((representation >> i) & 1);
            if (i % 4) cout << '\n';
        }
    }

    void piece::flip_piece_clockwise() {
        ++rotation %= 4;
        representation = representation_table[rotation];
    }

    void piece::flip_piece_counterwise() {
        --rotation %= 4;
        representation = representation_table[rotation];
    }

    // --------------------------------------------
    NULL_piece::NULL_piece() {
        piece_type = PieceType::Empty;
        rotation = NULL;
        representation = 0;
        placed = NULL;
    }

    // --------------------------------------------
    I_piece::I_piece() {
        piece_type = PieceType::I;
        rotation = 0;
        representation = representation_table[rotation];
        placed = false;
    }

    // --------------------------------------------
    J_piece::J_piece() {
        piece_type = PieceType::J;
        rotation = 0;
        representation = representation_table[rotation];
        placed = false;
    }

    // --------------------------------------------
    L_piece::L_piece() {
        piece_type = PieceType::L;
        rotation = 0;
        representation = representation_table[rotation];
        placed = false;
    }

    // --------------------------------------------
    // Note: square piece rotations should all be the same
    O_piece::O_piece() {
        piece_type = PieceType::O;
        rotation = 0;
        representation = representation_table[rotation];
        placed = false;
    }

    // --------------------------------------------
    S_piece::S_piece() {
        piece_type = PieceType::S;
        rotation = 0;
        representation = representation_table[rotation];
        placed = false;
    }

    // --------------------------------------------
    T_piece::T_piece() {
        piece_type = PieceType::T;
        rotation = 0;
        representation = representation_table[rotation];
        placed = false;
    }

    // --------------------------------------------
    Z_piece::Z_piece() {
        piece_type = PieceType::Z;
        rotation = 0;
        representation = representation_table[rotation];
        placed = false;
    }
}