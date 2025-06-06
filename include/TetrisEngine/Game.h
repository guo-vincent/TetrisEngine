/* 
Class for defining a Game Class, dealing with multiple boards
*/
#include <Board.h>
#include <vector>
#include <type_traits>
#include <utility>
#include <random>

namespace tetris {
    class Game {
        public:
            template <typename... OtherBoards>
            Game(OtherBoards&&... other) : 
                m_boards(),
                m_rng(std::random_device{}())
            {
                static_assert(sizeof...(other) >= 1, "At least one board required");
                static_assert((std::is_same_v<std::decay_t<OtherBoards>, Board> && ...), 
                            "All arguments must be Boards");
                m_boards.reserve(sizeof...(other));
                (m_boards.emplace_back(std::forward<OtherBoards>(other), m_rng), ...);
            }

            Board& getHost() noexcept { return m_boards[0]; }
            const Board& getHost() const noexcept { return m_boards[0]; }

            std::mt19937& getRNG() noexcept { return m_rng; }

        private:
            std::vector<Board> m_boards;
            std::mt19937 m_rng;
    };
}