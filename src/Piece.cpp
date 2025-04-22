#include "../include/TetrisEngine/Piece.h"
#include <cmath>

/**
 * @file Piece.cpp
 * @brief Implementation of Tetris pieces and their behaviors.
 * 
 * This file contains the implementation of various Tetris pieces, including
 * their constructors, rotation methods, and utility functions. Each piece
 * type (I, J, L, O, S, T, Z) is represented as a class with specific attributes
 * and behaviors.
 */

/**
 * @class piece
 * @brief Base class for all Tetris pieces.
 * 
 * The `piece` class provides common functionality for all Tetris pieces,
 * including rotation and printing. It serves as a base class for specific
 * piece types.
 */
namespace tetris {
    using std::endl;
    using std::cout;


    /**
     * @brief Default constructor for the `piece` class.
     * Initializes the piece as empty with no rotation or representation.
     */

    piece::piece() {
        piece_type = PieceType::Empty;
        rotation = NULL;
        representation = 0;
        placed = NULL;
    }
    
    /**
     * @brief Prints the current representation of the piece.
     * 
     * Outputs the binary representation of the piece to the console, formatted
     * as a 4x4 grid.
     */

    void piece::print_piece() {
        for (size_t i = 15; i >= 0; i--) {
            cout << ((representation >> i) & 1);
            if (i % 4) cout << '\n';
        }
    }
    
    /**
     * @brief Rotates the piece clockwise.
     * Updates the rotation state and representation of the piece.
     */

    void piece::flip_piece_clockwise() {
        ++rotation %= 4;
        representation = representation_table[rotation];
    }
    
    /**
     * @brief Rotates the piece counterclockwise.
     * Updates the rotation state and representation of the piece.
     */

    void piece::flip_piece_counterwise() {
        --rotation %= 4;
        representation = representation_table[rotation];
    }

    /**
     * @class NULL_piece
     * @brief Represents an empty Tetris piece.
     * 
     * The `NULL_piece` class is used to represent an empty or uninitialized
     * piece in the game.
     */

    NULL_piece::NULL_piece() {
        piece_type = PieceType::Empty;
        rotation = NULL;
        representation = 0;
        placed = NULL;
    }

    /**
     * @class I_piece
     * @brief Represents the "I" Tetris piece.
     * 
     * The `I_piece` class initializes the piece with its specific type,
     * rotation, and representation.
     */

    I_piece::I_piece() {
        piece_type = PieceType::I;
        rotation = 0;
        representation = representation_table[rotation];
        placed = false;
    }

    /**
     * @class J_piece
     * @brief Represents the "J" Tetris piece.
     * 
     * The `J_piece` class initializes the piece with its specific type,
     * rotation, and representation.
     */

    J_piece::J_piece() {
        piece_type = PieceType::J;
        rotation = 0;
        representation = representation_table[rotation];
        placed = false;
    }

    /**
     * @class L_piece
     * @brief Represents the "L" Tetris piece.
     * 
     * The `L_piece` class initializes the piece with its specific type,
     * rotation, and representation.
     */

    L_piece::L_piece() {
        piece_type = PieceType::L;
        rotation = 0;
        representation = representation_table[rotation];
        placed = false;
    }

    /**
     * @class O_piece
     * @brief Represents the "O" Tetris piece.
     * 
     * The `O_piece` class initializes the piece with its specific type,
     * rotation, and representation. Note that all rotations for this piece
     * are identical.
     */

    O_piece::O_piece() {
        piece_type = PieceType::O;
        rotation = 0;
        representation = representation_table[rotation];
        placed = false;
    }

    /**
     * @class S_piece
     * @brief Represents the "S" Tetris piece.
     * 
     * The `S_piece` class initializes the piece with its specific type,
     * rotation, and representation.
     */

    S_piece::S_piece() {
        piece_type = PieceType::S;
        rotation = 0;
        representation = representation_table[rotation];
        placed = false;
    }

    /**
     * @class T_piece
     * @brief Represents the "T" Tetris piece.
     * 
     * The `T_piece` class initializes the piece with its specific type,
     * rotation, and representation.
     */

    T_piece::T_piece() {
        piece_type = PieceType::T;
        rotation = 0;
        representation = representation_table[rotation];
        placed = false;
    }

    /**
     * @class Z_piece
     * @brief Represents the "Z" Tetris piece.
     * 
     * The `Z_piece` class initializes the piece with its specific type,
     * rotation, and representation.
     */

    Z_piece::Z_piece() {
        piece_type = PieceType::Z;
        rotation = 0;
        representation = representation_table[rotation];
        placed = false;
    }
}