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
    Board::Board(std::mt19937& rng_ref) : rng(rng_ref),
    grab_bag{ {PieceType::I, PieceType::J, PieceType::L, PieceType::O, PieceType::S, PieceType::T, PieceType::Z }}, 
    grab_bag_next{ {PieceType::I, PieceType::J, PieceType::L, PieceType::O, PieceType::S, PieceType::T, PieceType::Z }} 
    {
        Reset();
    }

    void Board::Reset() {
        InitializeGrid();
        currentPiece.reset();
        held_piece.reset();
        isGameOverFlag = false;
        score = 0;
        linesClearedTotal = 0;
        index = 0;
        back_to_back = 0;
        lastMoveWasRotation = false;
        last_piece_is_none = true;
        canHold = true;
        SpawnRandomPiece(); // Spawns first piece after initilizing
    }

    PieceType Board::GetHeldPieceType() const {
        if (held_piece) {
            return held_piece->GetType();
        }
        return PieceType::EMPTY;
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
        lastMoveWasRotation = false;
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
            lastMoveWasRotation = false;
            return true;
        }
        return false;
    }

    bool Board::RotateActivePiece(RotationDirection direction) {
        if (!currentPiece) return false;
        PieceType type = currentPiece->GetType();
        if (type == PieceType::O) return true; // O doesn't rotate

        RotationState from_rot = currentPiece->GetCurrentRotation();
        RotationState to_rot;
        if (direction == RotationDirection::CLOCKWISE) {
            to_rot = static_cast<RotationState>((static_cast<int>(from_rot) + 1) % 4);
        } else if (direction == RotationDirection::COUNTER_CLOCKWISE) {
            to_rot = static_cast<RotationState>((static_cast<int>(from_rot) + 3) % 4);
        } else {
            to_rot = static_cast<RotationState>((static_cast<int>(from_rot) + 2) % 4);
        }
        const auto& kicks = GetSrsKickData(type, from_rot, to_rot);
        uint16_t new_repr = currentPiece->GetRepresentation(to_rot);

        for (const Point& kick : kicks) {
            Point test_pos = currentPieceTopLeftPos + kick;
            if (IsValidPosition(new_repr, test_pos)) {
                currentPiece->SetCurrentRotation(to_rot);
                currentPieceTopLeftPos = test_pos;
                lastMoveWasRotation = true;
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
            lastMoveWasRotation = false;
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

        // Check for spins BEFORE clearing lines and BEFORE writing to the grid
        int isTSpin = IsTSpin();
        bool isAllMiniSpin = IsAllMiniSpin();

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

        // Clear lines and get count
        int lines = ClearFullLines();

        // Score calculation
        int baseScore = 0;
        bool isB2BEligible = false;

        if (isTSpin == 2) {         // full t-spin
            if (lines == 0) baseScore = 400;
            else if (lines == 1) {
                baseScore = 800;
                isB2BEligible = true;
            }
            else if (lines == 2) {
                baseScore = 1200;
                isB2BEligible = true;
            }
            else if (lines == 3) {
                baseScore = 1600;
                isB2BEligible = true;
            }
        } else if (isTSpin == 1) {  // t-spin mini
            if (lines == 0) baseScore = 100;
            else if (lines == 1) {
                baseScore = 200;
                isB2BEligible = true;
            }
            else if (lines == 2) {
                baseScore = 400;
                isB2BEligible = true;
            }
        } else {
            if (lines == 1) baseScore = 100;
            else if (lines == 2) baseScore = 300;
            else if (lines == 3) baseScore = 500;
            else if (lines == 4) {
                baseScore = 800;
                isB2BEligible = true;
            }
            if (isAllMiniSpin) {
                baseScore += 50;
                isB2BEligible = true;
            }
        }

        // Apply B2B bonus
        if (isB2BEligible) {
            if (back_to_back > 0) {
                baseScore = baseScore * 3 / 2; // 1.5x bonus
            }
            back_to_back++;
        } else if (lines > 0) {
            // Reset B2B if cleared lines without T-spin/Tetris
            back_to_back = 0;
        }

        score += baseScore;
        linesClearedTotal += lines;

        // Reset current piece
        currentPiece.reset();

        // Since we've updated the board, we can now hold a new piece
        canHold = true;

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
        int row = (type == PieceType::I) ? 19 : 20;
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
        // I-piece SRS+ clockwise kicks
        static const std::vector<Point> I_0R = {{0,0}, {1,0}, {-2,0}, {-2,-1}, {1,2}};
        static const std::vector<Point> I_R2 = {{0,0}, {-1,0}, {2,0}, {-1,2}, {2,-1}};
        static const std::vector<Point> I_2L = {{0,0}, {2,0}, {-1,0}, {2,1}, {-1,-2}};
        static const std::vector<Point> I_L0 = {{0,0}, {1,0}, {-2,0}, {1,-2}, {-2,1}};
        // JLSTZ SRS counter-clockwise kicks
        static const std::vector<Point> JLSTZ_R0 = {{0,0}, {1,0}, {1,-1}, {0,2}, {1,2}};
        static const std::vector<Point> JLSTZ_2R = {{0,0}, {-1,0}, {-1,1}, {0,-2}, {-1,-2}};
        static const std::vector<Point> JLSTZ_L2 = {{0,0}, {-1,0}, {-1,-1}, {0,2}, {-1,2}};
        static const std::vector<Point> JLSTZ_0L = {{0,0}, {1,0}, {1,1}, {0,-2}, {1,-2}};
        // I-piece SRS+ counter-clockwise kicks
        static const std::vector<Point> I_R0 = {{0,0}, {-1,0}, {2,0}, {-1,-2}, {2,1}};
        static const std::vector<Point> I_2R = {{0,0}, {-2,0}, {1,0}, {-2,1}, {1,-2}};
        static const std::vector<Point> I_L2 = {{0,0}, {1,0}, {-2,0}, {1,2}, {-2,-1}};
        static const std::vector<Point> I_0L = {{0,0}, {-1,0}, {2,0}, {2,-1}, {-1,2}};
        // All-piece SRS+ 180 kicks
        static const std::vector<Point> A_02 = {{0,0}, {0,1}, {1,1}, {-1,1}, {1,0}, {-1,0}};
        static const std::vector<Point> A_20 = {{0,0}, {0,-1}, {-1,-1}, {1,-1}, {-1,0}, {1,0}};
        static const std::vector<Point> A_RL = {{0,0}, {1,0}, {1,2}, {1,1}, {0,2}, {0,1}};
        static const std::vector<Point> A_LR = {{0,0}, {-1,0}, {-1,2}, {-1,1}, {0,2}, {0,1}};

        static const std::vector<Point> empty;

        if (from == RotationState::STATE_0 && to == RotationState::STATE_2) return A_02;
        if (from == RotationState::STATE_2 && to == RotationState::STATE_0) return A_20;
        if (from == RotationState::STATE_R && to == RotationState::STATE_L) return A_RL;
        if (from == RotationState::STATE_L && to == RotationState::STATE_R) return A_LR;
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

    std::vector<PieceType> Board::GetNextQueue() const {
        // look at next 5 pieces
        std::vector<PieceType> queue;
        for (int i = 0; i < 5; i++) {
            size_t look_index = index + i;
            if (look_index < grab_bag.size()){
                queue.push_back(grab_bag[look_index]);
            } else {
                queue.push_back(grab_bag_next[look_index%grab_bag.size()]);
            }
        }
        // std::cout << "Bags: ";
        // for (auto piece : grab_bag){
        //     std::cout << static_cast<int>(piece) << " ";
        // }
        // for (auto piece : grab_bag_next){
        //     std::cout << static_cast<int>(piece) << " ";
        // }
        // std::cout << "\n";
        // std::cout << "Index: " << index << "\n";
        return queue;
    }

    void Board::HoldPiece() {
        if (!currentPiece) return;

        PieceType currentType = currentPiece->GetType();
        // if std::unique_ptr<Piece> held_piece is empty, transfer ownership of obj to next piece
        // we make a new Tetris::Piece object as the nullptr because we need coordinates to start at the top
        if (!held_piece) {
            held_piece = CreatePieceByType(currentPiece->GetType());
            currentPiece.reset();
            SpawnRandomPiece();
        } else { // Piece is not empty
            if (!canHold) return;
            PieceType heldType = held_piece->GetType();
        
            // Store current piece in hold
            held_piece = CreatePieceByType(currentType);
            
            // Spawn held piece at correct position
            currentPiece = CreatePieceByType(heldType);
            Point spawnPos = CalculateSpawnPosition(heldType);
            currentPieceTopLeftPos = spawnPos;
            lastMoveWasRotation = false;

            // Game over if spawn position is blocked
            if (!IsValidPosition(
                currentPiece->GetCurrentRepresentation(),
                spawnPos
            )) {
                isGameOverFlag = true;
            }
        }

        canHold = false;
    }

    int Board::IsTSpin() const {
        if (!currentPiece || currentPiece->GetType() != PieceType::T || !lastMoveWasRotation) {
            return 0;
        }

        const RotationState rotation  = currentPiece->GetCurrentRotation();
        const Point center = currentPieceTopLeftPos + Point{1, 1};

        // Corners around the center (mirrored on x-axis)
        std::array<Point, 4> corners = {
            center + Point{-1, 1},  // top-left         
            center + Point{1, 1},   // top-right        
            center + Point{1, -1},  // bottom-right     
            center + Point{-1, -1}  // bottom-left       
        };

        // Permute corners to line up with piece rotation
        std::rotate(corners.begin(), corners.begin() + static_cast<int>(rotation), corners.end());

        // Test on top side and bottom side seperately
        int topOccupied = 0;
        int bottomOccupied = 0;
        for (size_t i = 0; i < corners.size(); i++) {
            Point p = corners[i];
            // Treat out-of-bounds as occupied
            if (p.x < 0 || p.x >= BOARD_WIDTH || p.y < 0 || p.y >= TOTAL_BOARD_HEIGHT ||
                GetCellState(p.x, p.y) != PieceType::EMPTY) {
                if (i < 2){
                    ++topOccupied;
                } else {
                    ++bottomOccupied;
                }
            }
        }

        // Determine t-spin quality
        if (topOccupied == 2 && bottomOccupied >= 1){
            return 2;
        }
        if ((bottomOccupied == 2 && topOccupied >= 1) || IsAllMiniSpin()){
            return 1;
        }

        return 0;
    }

    bool Board::IsAllMiniSpin() const {
        if (!currentPiece || currentPiece->GetType() == PieceType::O || !lastMoveWasRotation) {
            return false;
        }

        static const std::vector<Point> tests = {{1,0}, {-1,0}, {0,1}, {0,-1}};
        uint16_t repr = currentPiece->GetCurrentRepresentation();

        for (const Point& test : tests) {
            Point test_pos = currentPieceTopLeftPos + test;
            if (IsValidPosition(repr, test_pos)) {
                return false;
            }
        }
        return true;
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