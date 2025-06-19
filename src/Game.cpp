#include "TetrisEngine/Game.h"
#include "TetrisEngine/Board.h"

namespace tetris {
    void Game::Reset() {
        for (size_t i = 0; i < playerCount(); i++) {
            getBoard(i).Reset();
        }

        gravityClock.reset();
    }

    void Game::TransferGarbage(size_t sendingPlayerID, int lines){
        size_t target_player = (sendingPlayerID) ? 0u : 1u; // targetting only considers 2 players 
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

    // This is so scuffed
    void Game::moveAllPiecesDown(int row) {
        for (int i = 0; i < row; i++) {  // Move one row at a time
            for (size_t j = 0; j < m_boards.size(); j++) {
                Board& board = *m_boards[j];
                // Skip boards in lock delay
                if (board.IsInLockDelay()) continue;
                
                if (board.HasActivePiece()) {
                    if (!board.MoveActivePiece(0, -1)) {
                        // If moving down fails, start lock delay if not already started
                        if (!board.IsInLockDelay()) {
                            board.StartLockDelay();
                        }
                    }
                }
            }
        }
    }

    void Game::Update() {
        static auto lastTime = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> deltaTime = now - lastTime;
        lastTime = now;

        gravityClock.update();
        for (std::unique_ptr<Board>& board : m_boards) {
            board->UpdateLockDelay(deltaTime.count());
        }
    }

} // namespace tetris