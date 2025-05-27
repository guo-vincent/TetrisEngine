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
    board.PrintBoard(true);  // Show hidden area

    // Move right
    board.MoveActivePiece(1, 0);
    std::cout << "After moving right:\n";
    board.PrintBoard(true);

    // Move left twice (should stop at left wall)
    board.MoveActivePiece(-1, 0);
    board.MoveActivePiece(-1, 0);
    std::cout << "After trying to move left twice:\n";
    board.PrintBoard(true);
}

void TestRotation() {
    std::cout << "\n=== Testing Rotation ===\n";
    Board board;
    board.SpawnNewPiece(PieceType::I);
    
    std::cout << "Initial I-piece:\n";
    board.PrintBoard(true);
    
    // Rotate clockwise
    board.RotateActivePiece(RotationDirection::CLOCKWISE);
    std::cout << "After clockwise rotation:\n";
    board.PrintBoard(true);
}

void TestLineClear() {
    std::cout << "\n=== Testing Line Clear ===\n";
    Board board;

    // Fill row 5 (visible area) with I-pieces to create a full line
    // I-piece in STATE_0 (horizontal) covers columns 0-3 at row 5
    board.SpawnNewPiece(PieceType::I);
    board.currentPieceTopLeftPos = {0, 5}; // Columns 0-3
    board.LockActivePiece();
    board.PrintBoard(true);

    // Second I-piece covers columns 4-7
    board.SpawnNewPiece(PieceType::I);
    board.currentPieceTopLeftPos = {4, 5};
    board.LockActivePiece();
    board.PrintBoard(true);

    // Third I-piece adjusted to fit within BOARD_WIDTH=10
    board.SpawnNewPiece(PieceType::O);
    board.currentPieceTopLeftPos = {7, 5}; // Columns 6-9
    board.LockActivePiece();

    // Verify the board state
    std::cout << "Before clear:\n";
    board.PrintBoard(true);

    // Spawn and drop an O-piece to trigger line clear
    board.SpawnNewPiece(PieceType::O);
    board.HardDropActivePiece();

    std::cout << "After clear:\n";
    board.PrintBoard(true);
    std::cout << "Lines cleared: " << board.GetLinesCleared() << "\n";
}

int main() {
    // TestBasicMovement();
    // TestRotation();
    // TestLineClear();
    return 0;
}