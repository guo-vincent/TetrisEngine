#include "../include/TetrisEngine/Board.h"
#include "../include/TetrisEngine/Piece.h"
#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <random>
#include <unordered_set>
#include <raylib.h>

namespace tetris {
    Board::Board() : rng(std::random_device{}()), grab_bag{ {PieceType::I, PieceType::J, PieceType::L, PieceType::O, PieceType::S, PieceType::T, PieceType::Z }}, 
                     grab_bag_next{ {PieceType::I, PieceType::J, PieceType::L, PieceType::O, PieceType::S, PieceType::T, PieceType::Z }} {
        Reset();
    }

    void Board::Reset() {
        InitializeGrid();
        currentPiece.reset();
        isGameOverFlag = false;
        score = 0;
        linesClearedTotal = 0;
        index = 0;
        last_piece_is_none = true;
    }

    bool Board::SpawnNewPiece(PieceType type) {
        auto piece = CreatePieceByType(type);
        if (!piece) return false;
        uint16_t repr = piece->GetCurrentRepresentation();
        RotationState spawnRotation = RotationState::STATE_0;
        piece->SetCurrentRotation(spawnRotation);
        Point spawnPos = CalculateSpawnPosition(type);

        // Game over check: if spawn position is invalid
        if (!IsValidPosition(repr, spawnPos)) {
            isGameOverFlag = true;
            return false;
        }

        currentPiece = std::move(piece);
        currentPieceTopLeftPos = spawnPos;
        last_piece_is_none = false;
        return true;
    }

    bool Board::SpawnRandomPiece() {
        index %= grab_bag.size();

        // On start, shuffle first next bag
        if (last_piece_is_none){
            std::shuffle(grab_bag_next.begin(), grab_bag_next.end(), rng);
        }

        // On bag emptied, copy next bag and shuffle new next bag
        if (index == 0) {
            grab_bag = grab_bag_next;
            std::shuffle(grab_bag_next.begin(), grab_bag_next.end(), rng);
        }

        // Pick and advance
        PieceType next = grab_bag[index++];
        last_piece = next;
        last_piece_is_none = false;
        return SpawnNewPiece(next);
    }

    bool Board::MoveActivePiece(int delta_x, int delta_y) {
        if (!currentPiece) return false;
        Point newPos = currentPieceTopLeftPos + Point{delta_x, delta_y};
        uint16_t repr = currentPiece->GetCurrentRepresentation();

        if (IsValidPosition(repr, newPos)) {
            currentPieceTopLeftPos = newPos;
            return true;
        }
        return false;
    }

    bool Board::RotateActivePiece(RotationDirection direction) {
        if (!currentPiece) return false;
        PieceType type = currentPiece->GetType();
        if (type == PieceType::O) return true; // O doesn't rotate

        RotationState from_rot = currentPiece->GetCurrentRotation();
        RotationState to_rot = (direction == RotationDirection::CLOCKWISE) ?
            static_cast<RotationState>((static_cast<int>(from_rot) + 1) % 4) :
            static_cast<RotationState>((static_cast<int>(from_rot) + 3) % 4);

        const auto& kicks = GetSrsKickData(type, from_rot, to_rot);
        uint16_t new_repr = currentPiece->GetRepresentation(to_rot);

        for (const Point& kick : kicks) {
            Point test_pos = currentPieceTopLeftPos + kick;
            if (IsValidPosition(new_repr, test_pos)) {
                currentPiece->SetCurrentRotation(to_rot);
                currentPieceTopLeftPos = test_pos;
                return true;
            }
        }
        return false;
    }

    void Board::HardDropActivePiece() {
        if (!currentPiece) return;
        Point pos = currentPieceTopLeftPos;
        uint16_t repr = currentPiece->GetCurrentRepresentation();
        while (IsValidPosition(repr, {pos.x, pos.y - 1})) {
            pos.y--;
        }
        if (!IsValidPosition(repr, pos)) return; // Avoid locking in an invalid position
        currentPieceTopLeftPos = pos;
        LockActivePiece();
    }

    void Board::LockActivePiece() {
        if (!currentPiece) return;
        uint16_t repr = currentPiece->GetCurrentRepresentation();
        int x = currentPieceTopLeftPos.x;
        int y = currentPieceTopLeftPos.y;

        // Write the piece to the grid (only once)
        for (int i = 0; i < 16; ++i) {
            if (repr & (1 << (15 - i))) {
                int row = y + (i / 4);
                int col = x + (i % 4);
                if (col >= 0 && col < BOARD_WIDTH && row >= 0 && row < TOTAL_BOARD_HEIGHT) {
                    grid[row * BOARD_WIDTH + col] = currentPiece->GetType();
                }
            }
        }

        // Clear lines and update score
        int lines = ClearFullLines();
        score += lines * 100;
        linesClearedTotal += lines;

        // Reset current piece
        currentPiece.reset();

        // Game over is checked in SpawnNewPiece, not here
    }

    int Board::ClearFullLines() {
        int lines = 0;
        // Iterate from bottom (row 0) to top (row 19)
        for (int row = 0; row < VISIBLE_BOARD_HEIGHT; ++row) {
            bool full = true;
            // Check if all cells in the row are filled
            for (int col = 0; col < BOARD_WIDTH; ++col) {
                if (grid[row * BOARD_WIDTH + col] == PieceType::EMPTY) {
                    full = false;
                    break;
                }
            }
            if (full) {
                // Shift all rows above DOWN by one
                for (int r = row; r < TOTAL_BOARD_HEIGHT - 1; ++r) {
                    std::copy_n(
                        &grid[(r + 1) * BOARD_WIDTH],  // Source: row above
                        BOARD_WIDTH,
                        &grid[r * BOARD_WIDTH]         // Destination: current row
                    );
                }
                // Clear the topmost row (hidden buffer)
                std::fill_n(
                    &grid[(TOTAL_BOARD_HEIGHT - 1) * BOARD_WIDTH],
                    BOARD_WIDTH,
                    PieceType::EMPTY
                );
                lines++;
                row--; // Re-check this row index after shifting
            }
        }
        return lines;
    }

    void Board::InitializeGrid() {
        std::fill(grid.begin(), grid.end(), PieceType::EMPTY);
    }

    Point Board::CalculateSpawnPosition(PieceType type) {
        int row = (type == PieceType::I) ? 21 : 20;
        return {3, row}; // Centered at column 3
    }

    bool Board::IsValidPosition(uint16_t repr, Point pos) const {
        for (int i = 0; i < 16; ++i) {
            if (repr & (1 << (15 - i))) {
                int c = pos.x + (i % 4);
                int r = pos.y + (i / 4);
                if (c < 0 || c >= BOARD_WIDTH || r < 0 || r >= TOTAL_BOARD_HEIGHT) {
                    return false;
                }
                if (grid[r * BOARD_WIDTH + c] != PieceType::EMPTY) {
                    return false;
                }
            }
        }
        return true;
    }

    std::unique_ptr<Piece> Board::CreatePieceByType(PieceType type) {
        switch (type) {
            case PieceType::I: return std::make_unique<PieceI>();
            case PieceType::J: return std::make_unique<PieceJ>();
            case PieceType::L: return std::make_unique<PieceL>();
            case PieceType::O: return std::make_unique<PieceO>();
            case PieceType::S: return std::make_unique<PieceS>();
            case PieceType::T: return std::make_unique<PieceT>();
            case PieceType::Z: return std::make_unique<PieceZ>();
            default: return nullptr;
        }
    }

    const std::vector<Point>& Board::GetSrsKickData(PieceType type, RotationState from, RotationState to) const {
        // JLSTZ SRS clockwise kicks
        static const std::vector<Point> JLSTZ_0R = {{0,0}, {-1,0}, {-1,1}, {0,-2}, {-1,-2}};
        static const std::vector<Point> JLSTZ_R2 = {{0,0}, {1,0}, {1,-1}, {0,2}, {1,2}};
        static const std::vector<Point> JLSTZ_2L = {{0,0}, {1,0}, {1,1}, {0,-2}, {1,-2}};
        static const std::vector<Point> JLSTZ_L0 = {{0,0}, {-1,0}, {-1,-1}, {0,2}, {-1,2}};
        // I-piece SRS clockwise kicks
        static const std::vector<Point> I_0R = {{0,0}, {-2,0}, {1,0}, {-2,-1}, {1,2}};
        static const std::vector<Point> I_R2 = {{0,0}, {-1,0}, {2,0}, {-1,2}, {2,-1}};
        static const std::vector<Point> I_2L = {{0,0}, {2,0}, {-1,0}, {2,1}, {-1,-2}};
        static const std::vector<Point> I_L0 = {{0,0}, {1,0}, {-2,0}, {1,-2}, {-2,1}};
        // JLSTZ SRS counter-clockwise kicks
        static const std::vector<Point> JLSTZ_R0 = {{0,0}, {1,0}, {1,-1}, {0,2}, {1,2}};
        static const std::vector<Point> JLSTZ_2R = {{0,0}, {-1,0}, {-1,1}, {0,-2}, {-1,-2}};
        static const std::vector<Point> JLSTZ_L2 = {{0,0}, {-1,0}, {-1,-1}, {0,2}, {-1,2}};
        static const std::vector<Point> JLSTZ_0L = {{0,0}, {1,0}, {1,1}, {0,-2}, {1,-2}};
        // I-piece SRS counter-clockwise kicks
        static const std::vector<Point> I_R0 = {{0,0}, {2,0}, {-1,0}, {2,1}, {-1,-2}};
        static const std::vector<Point> I_2R = {{0,0}, {1,0}, {-2,0}, {1,-2}, {-2,1}};
        static const std::vector<Point> I_L2 = {{0,0}, {-2,0}, {1,0}, {-2,-1}, {1,2}};
        static const std::vector<Point> I_0L = {{0,0}, {-1,0}, {2,0}, {-1,2}, {2,-1}};

        static const std::vector<Point> empty;

        if (type == PieceType::I) {
            if (from == RotationState::STATE_0 && to == RotationState::STATE_R) return I_0R;
            if (from == RotationState::STATE_R && to == RotationState::STATE_0) return I_R0;
            if (from == RotationState::STATE_R && to == RotationState::STATE_2) return I_R2;
            if (from == RotationState::STATE_2 && to == RotationState::STATE_R) return I_2R;
            if (from == RotationState::STATE_2 && to == RotationState::STATE_L) return I_2L;
            if (from == RotationState::STATE_L && to == RotationState::STATE_2) return I_L2;
            if (from == RotationState::STATE_L && to == RotationState::STATE_0) return I_L0;
            if (from == RotationState::STATE_0 && to == RotationState::STATE_L) return I_0L;
        } else if (type != PieceType::O) {
            if (from == RotationState::STATE_0 && to == RotationState::STATE_R) return JLSTZ_0R;
            if (from == RotationState::STATE_R && to == RotationState::STATE_0) return JLSTZ_R0;
            if (from == RotationState::STATE_R && to == RotationState::STATE_2) return JLSTZ_R2;
            if (from == RotationState::STATE_2 && to == RotationState::STATE_R) return JLSTZ_2R;
            if (from == RotationState::STATE_2 && to == RotationState::STATE_L) return JLSTZ_2L;
            if (from == RotationState::STATE_L && to == RotationState::STATE_2) return JLSTZ_L2;
            if (from == RotationState::STATE_L && to == RotationState::STATE_0) return JLSTZ_L0;
            if (from == RotationState::STATE_0 && to == RotationState::STATE_L) return JLSTZ_0L;
        }
        return empty;
    }

    PieceType Board::GetCellState(int col, int row_from_bottom) const {
        // Add bounds checking
        if (col < 0 || col >= BOARD_WIDTH || row_from_bottom < 0 || row_from_bottom >= TOTAL_BOARD_HEIGHT) {
            return PieceType::EMPTY;
        }
        return grid[row_from_bottom * BOARD_WIDTH + col];
    }

    Color tetris::Board::GetColorForPieceType(tetris::PieceType pt) const {
        switch (pt) {
            case PieceType::I: return SKYBLUE;
            case PieceType::J: return BLUE;
            case PieceType::L: return ORANGE;
            case PieceType::O: return YELLOW;
            case PieceType::S: return GREEN;
            case PieceType::T: return PURPLE;
            case PieceType::Z: return RED;
            default: return GRAY;
        }
    }

    // based on board state return 1D array of the visible board including active piece
    std::vector<PieceType> Board::GetRenderableState() const {
        std::vector<PieceType> state(VISIBLE_BOARD_HEIGHT * BOARD_WIDTH, PieceType::EMPTY);

        // Copy locked cells in reverse row order (top row first)
        for (int state_row = 0; state_row < VISIBLE_BOARD_HEIGHT; ++state_row) {
            int grid_row = VISIBLE_BOARD_HEIGHT - 1 - state_row;
            for (int col = 0; col < BOARD_WIDTH; ++col) {
                PieceType pt = GetCellState(col, grid_row);
                state[state_row * BOARD_WIDTH + col] = pt;
            }
        }

        // Overlay active piece
        if (currentPiece) {
            uint16_t repr = currentPiece->GetCurrentRepresentation();
            int x = currentPieceTopLeftPos.x;
            int y = currentPieceTopLeftPos.y;
            PieceType activeType = currentPiece->GetType();

            for (int i = 0; i < 16; ++i) {
                if (repr & (1 << (15 - i))) {
                    int col = x + (i % 4);
                    int grid_row = y + (i / 4);
                    if (grid_row >= 0 && grid_row < VISIBLE_BOARD_HEIGHT) {
                        int state_row = VISIBLE_BOARD_HEIGHT - 1 - grid_row;
                        if (col >= 0 && col < BOARD_WIDTH) {
                            state[state_row * BOARD_WIDTH + col] = activeType;
                        }
                    }
                }
            }
        }

        return state;
    }

    void Board::PrintBoardText(bool show_hidden = false) const {
        const int start_row = show_hidden ? TOTAL_BOARD_HEIGHT - 1 : VISIBLE_BOARD_HEIGHT - 1;
        const int end_row = 0;

        // Track active piece blocks
        std::unordered_set<int> active_piece_cells;
        if (currentPiece) {
            uint16_t repr = currentPiece->GetCurrentRepresentation();
            int x = currentPieceTopLeftPos.x;
            int y = currentPieceTopLeftPos.y;
            for (int i = 0; i < 16; ++i) {
                if (repr & (1 << (15 - i))) {
                    int col = x + (i % 4);
                    int row = y + (i / 4);
                    active_piece_cells.insert(row * BOARD_WIDTH + col);
                }
            }
        }

        for (int row = start_row; row >= end_row; --row) {
            std::cout << std::setw(2) << row << " ";
            for (int col = 0; col < BOARD_WIDTH; ++col) {
                int idx = row * BOARD_WIDTH + col;
                if (active_piece_cells.count(idx)) {
                    // Draw active piece
                    char c = static_cast<char>('A' + static_cast<int>(currentPiece->GetType()) - 1);
                    std::cout << c << " ";
                } else {
                    // Draw grid
                    PieceType pt = GetCellState(col, row);
                    char c = (pt != PieceType::EMPTY) ? static_cast<char>('A' + static_cast<int>(pt) - 1) : '.';
                    std::cout << c << " ";
                }
            }
            std::cout << "\n";
        }
        std::cout << "    ";
        for (int col = 0; col < BOARD_WIDTH; ++col) std::cout << col << " ";
        std::cout << "\n\n";
    }

    void Board::PrintBoard(const int screenWidth, const int screenHeight, bool show_hidden = false) const {
        const int start_row = show_hidden ? TOTAL_BOARD_HEIGHT - 1 : VISIBLE_BOARD_HEIGHT - 1;
        const int end_row = 0;

        int cellSize = std::min(screenWidth / BOARD_WIDTH, screenHeight / VISIBLE_BOARD_HEIGHT);

        int offsetX = (screenWidth - (cellSize * BOARD_WIDTH)) / 2;
        int offsetY = (screenHeight - (cellSize * VISIBLE_BOARD_HEIGHT)) / 2;

        // Track active piece blocks
        std::unordered_set<int> active_piece_cells;
        if (currentPiece) {
            uint16_t repr = currentPiece->GetCurrentRepresentation();
            int x = currentPieceTopLeftPos.x;
            int y = currentPieceTopLeftPos.y;
            for (int i = 0; i < 16; ++i) {
                if (repr & (1 << (15 - i))) {
                    int col = x + (i % 4);
                    int row = y + (i / 4);
                    active_piece_cells.insert(row * BOARD_WIDTH + col);
                }
            }
        }

        for (int row = start_row; row >= end_row; --row) {
            for (int col = 0; col < BOARD_WIDTH; ++col) {
                int idx = row * BOARD_WIDTH + col;
                int posX = offsetX + col * cellSize;
                int posY = offsetY + row * cellSize;
                if (active_piece_cells.count(idx)) {
                    // Draw active piece
                    PieceType pt = GetCellState(col, row);
                    Color color = (pt != PieceType::EMPTY) ? GetColorForPieceType(pt) : GRAY;
                    DrawRectangle(posX, posY, cellSize - 1, cellSize - 1, color);
                } else {
                    // Draw grid
                    DrawRectangle(posX, posY, cellSize - 1, cellSize - 1, GRAY);
                }
            }
        }
    }
} // namespace tetris