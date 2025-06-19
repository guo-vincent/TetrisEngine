#include "TetrisEngine/Controls.h"

namespace tetris {

void ControlLeft(Board& board){
    board.MoveActivePiece(-1, 0);
}

void ControlRight(Board& board){
    board.MoveActivePiece(1, 0);
}

void ControlRotateCW(Board& board){
    board.RotateActivePiece(RotationDirection::CLOCKWISE);
}

void ControlRotateCCW(Board& board){
    board.RotateActivePiece(RotationDirection::COUNTER_CLOCKWISE);
}

void ControlRotate180(Board& board){
    board.RotateActivePiece(RotationDirection::ONE_EIGHTY);
}

void ControlHold(Board& board){
    board.HoldPiece();
}

void ControlSoftDrop(Board& board){
    board.MoveActivePiece(0, -1);
}

void ControlHardDrop(Board& board){
    board.HardDropActivePiece();
}

}   // namespace tetris