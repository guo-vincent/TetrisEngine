#include "../include/TetrisEngine/Game.h"
#include "../include/TetrisEngine/Board.h"

namespace tetris {
    void Game::Reset() {
        for (size_t i = 0; i < playerCount(); i++) {
            getBoard(i).Reset();
        }
    }

    void Game::TransferGarbage(int sendingPlayerID, int lines){
        int target_player = (sendingPlayerID) ? 0 : 1; // targetting only considers 2 players 
        Board &target_board = getBoard(target_player);

        // cancel pending garbage
        while (lines != 0 && !pending_garbage_queues[sendingPlayerID].empty()) {
            if (lines >= pending_garbage_queues[sendingPlayerID].front()){
                lines -= pending_garbage_queues[sendingPlayerID].front();
                pending_garbage_queues[sendingPlayerID].pop();
            } else {
                pending_garbage_queues[sendingPlayerID].front() -= lines;
                lines = 0;
            }
        }

        // send lines to other player
        if (lines > 0) {
            pending_garbage_queues[target_player].push(lines);

            // wait 20 frames (or 500ms?)
            target_board.AddGarbageToQueue(pending_garbage_queues[target_player].front());
            pending_garbage_queues[target_player].pop();
        }
    }

} // namespace tetris