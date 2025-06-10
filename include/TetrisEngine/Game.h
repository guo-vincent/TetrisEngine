#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include <vector>
#include <random>

namespace tetris {

class Game {
public:
    explicit Game(size_t numPlayers)
        : m_rng(std::random_device{}())
    {
        m_boards.reserve(numPlayers);
        for (size_t i = 0; i < numPlayers; ++i) {
            m_boards.emplace_back(m_rng);  // Board requires reference to RNG
        }
    }

    Board& getBoard(size_t index) { return m_boards.at(index); }
    const Board& getBoard(size_t index) const { return m_boards.at(index); }

    Board& getHost() { return m_boards.at(0); }
    const Board& getHost() const { return m_boards.at(0); }

    std::mt19937& getRNG() noexcept { return m_rng; }

    size_t playerCount() const noexcept { return m_boards.size(); }

private:
    std::vector<Board> m_boards;
    std::mt19937 m_rng;
};

} // namespace tetris

#endif GAME_H