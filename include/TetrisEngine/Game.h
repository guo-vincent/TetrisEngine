#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include <vector>
#include <memory>
#include <random>
#include <stdexcept>

namespace tetris {

class Game {
    public:
        Game() : m_seed(std::random_device{}()) {}

        explicit Game(size_t numPlayers) : m_seed(std::random_device{}()) {
            for (size_t i = 0; i < numPlayers; ++i) {
                addPlayer();
            }
        }

        void addPlayer() {
            m_boards.emplace_back(std::make_unique<Board>(m_seed));
        }

        // I doubt we'll be using const Game, but here you go
        Board& getBoard(size_t index) {
            if (index >= m_boards.size()) throw std::out_of_range("Invalid board index");
            return *m_boards.at(index);
        }

        const Board& getBoard(size_t index) const {
            if (index >= m_boards.size()) throw std::out_of_range("Invalid board index");
            return *m_boards.at(index);
        }

        // Host is first player
        Board& getHost() {
            if (m_boards.empty()) throw std::runtime_error("No host board exists");
            return *m_boards.at(0);
        }

        const Board& getHost() const {
            if (m_boards.empty()) throw std::runtime_error("No host board exists");
            return *m_boards.at(0);
        }

        const unsigned int getRNG() noexcept { return m_seed; }

        size_t playerCount() const noexcept { return m_boards.size(); }

    private:
        std::vector<std::unique_ptr<Board>> m_boards;
        unsigned int m_seed;
    };
} // namespace tetris

#endif GAME_H
