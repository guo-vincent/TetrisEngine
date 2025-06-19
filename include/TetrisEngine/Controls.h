#ifndef Controls_H
#define Controls_H

#include "Board.h"

namespace tetris {

void ControlLeft(Board& board);
void ControlRight(Board& board);

void ControlRotateCW(Board& board);
void ControlRotateCCW(Board& board);
void ControlRotate180(Board& board);

void ControlHold(Board& board);

void ControlSoftDrop(Board& board);
void ControlHardDrop(Board& board);

}  // namespace tetris

#endif Controls_H