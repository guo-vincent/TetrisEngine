#include "../include/TetrisEngine/Board.h"
#include <iostream>
#include <limits>

using namespace tetris;

void TestBasicMovement() {
    std::cout << "=== Testing Basic Movement ===\n";
    Board board;
    
    // Spawn J piece
    board.SpawnNewPiece(PieceType::J);
    std::cout << "Initial spawn:\n";
    board.PrintBoardText(true);  // Show hidden area

    // Move right
    board.MoveActivePiece(1, 0);
    std::cout << "After moving right:\n";
    board.PrintBoardText(true);

    // Move left twice (should stop at left wall)
    board.MoveActivePiece(-1, 0);
    board.MoveActivePiece(-1, 0);
    std::cout << "After trying to move left twice:\n";
    board.PrintBoardText(true);
}

void TestRotation() {
    std::cout << "\n=== Testing Rotation ===\n";
    Board board;
    board.SpawnNewPiece(PieceType::I);
    
    std::cout << "Initial I-piece:\n";
    board.PrintBoardText(true);
    
    // Rotate clockwise
    board.RotateActivePiece(RotationDirection::CLOCKWISE);
    std::cout << "After clockwise rotation:\n";
    board.PrintBoardText(true);
}

void TestLineClear() {
    std::cout << "\n=== Testing Line Clear ===\n";
    Board board;

    // Fill row 5 (visible area) with I-pieces to create a full line
    // I-piece in STATE_0 (horizontal) covers columns 0-3 at row 5
    board.SpawnNewPiece(PieceType::I);
    board.MoveActivePiece(-3,0); // Columns 0-3
    board.HardDropActivePiece();
    board.PrintBoardText(true);

    // Second I-piece covers columns 4-7
    board.SpawnNewPiece(PieceType::I);
    board.MoveActivePiece(3,0);
    board.HardDropActivePiece();
    board.PrintBoardText(true);

    // Verify the board state
    std::cout << "Before clear:\n";
    board.PrintBoardText(true);

    // Spawn and drop an O-piece to trigger line clear
    board.SpawnNewPiece(PieceType::O);
    board.HardDropActivePiece();

    std::cout << "After clear:\n";
    board.PrintBoardText(true);
    std::cout << "Lines cleared: " << board.GetLinesCleared() << "\n";
}

int main() {
    // TestBasicMovement();
    // TestRotation();
    // TestLineClear();
    return 0;
}