#pragma once

#include <array>
#include <bit>
#include <bitset>
#include <cstdint>
#include <ranges>
#include <set>
#include <unordered_set>
#include <vector>

#include "Board.hpp"
#include "Piece.hpp"
#include "RotationSystems.hpp"
#include "ShaktrisConstants.hpp"
#include "Utility.hpp"
#include <immintrin.h>

// precomputed piece movements

/// <summary>
/// this function returns the size of the zero_g_starts
/// </summary>
/// <returns>size_t for the zero_g_starts of any given piece</returns>
consteval static size_t zero_g_starts_size(const PieceType p) {
    Piece initial_piece = Piece(p);

    auto collides = [](const Piece& piece) {

        for (auto& mino : piece.minos) {
            int x_pos = mino.x + piece.position.x;
            if (x_pos < 0 || x_pos >= Board::width)
                return true;

            int y_pos = mino.y + piece.position.y;
            if (y_pos < 0 || y_pos >= Board::height)
                return true;
        }

        return false;
        };

    auto shift = [&](Piece& piece, int dir) {
        piece.position.x += dir;

        if (collides(piece))
            piece.position.x -= dir;
        else
            piece.spin = spinType::null;
        };

    std::vector<Piece> open_nodes;
    open_nodes.reserve(150);
    std::vector<Piece> next_nodes;
    next_nodes.reserve(150);
    std::vector<bool> visited = std::vector<bool>(6444);

    std::vector<Piece> valid_moves;
    valid_moves.reserve(150);

    // rotations
    for (int r = 0; r < (int)RotationDirections_N; r++) {
        open_nodes.emplace_back(initial_piece);
        if (initial_piece.type != PieceType::O)
            initial_piece.rotate(TurnDirection::Right);

        // lateral movement
        while (open_nodes.size() > 0) {
            // expand edges
            for (const auto& piece : open_nodes) {
                size_t h = piece.compact_hash();
                if (visited[h])
                    continue;
                // mark node as visited
                visited[h] = true;

                valid_moves.push_back(piece);

                Piece new_piece = piece;
                shift(new_piece, -1);
                next_nodes.emplace_back(new_piece);

                new_piece = piece;
                shift(new_piece, 1);
                next_nodes.emplace_back(new_piece);
            }
            open_nodes = next_nodes;
            next_nodes.clear();
        }
    }

    return valid_moves.size();
};



// returns the zero g starts for the given piece
template<PieceType p>
consteval static auto zero_g_starts() {

    constexpr std::array<size_t, static_cast<size_t>(PieceType::PieceTypes_N)> zero_g_start_size = {
        zero_g_starts_size(PieceType::S),
        zero_g_starts_size(PieceType::Z),
        zero_g_starts_size(PieceType::J),
        zero_g_starts_size(PieceType::L),
        zero_g_starts_size(PieceType::T),
        zero_g_starts_size(PieceType::O),
        zero_g_starts_size(PieceType::I),
    };

    Piece initial_piece = Piece(p);

    auto collides = [](const Piece& piece) {

        for (auto& mino : piece.minos) {
            int x_pos = mino.x + piece.position.x;
            if (x_pos < 0 || x_pos >= Board::width)
                return true;

            int y_pos = mino.y + piece.position.y;
            if (y_pos < 0 || y_pos >= Board::height)
                return true;
        }

        return false;
        };

    auto shift = [&](Piece& piece, int dir) {
        piece.position.x += dir;

        if (collides(piece))
            piece.position.x -= dir;
        else
            piece.spin = spinType::null;
        };

    std::vector<Piece> open_nodes;
    open_nodes.reserve(150);
    std::vector<Piece> next_nodes;
    next_nodes.reserve(150);
    std::vector<bool> visited = std::vector<bool>(6444);

    std::vector<Piece> valid_moves;
    valid_moves.reserve(150);

    // rotations
    for (int r = 0; r < 4; r++) {
        open_nodes.emplace_back(initial_piece);
        if (initial_piece.type != PieceType::O)
            initial_piece.rotate(TurnDirection::Right);

        // lateral movement
        while (open_nodes.size() > 0) {
            // expand edges
            for (const auto& piece : open_nodes) {
                size_t h = piece.compact_hash();
                if (visited[h])
                    continue;
                // mark node as visited
                visited[h] = true;

                valid_moves.push_back(piece);

                Piece new_piece = piece;
                shift(new_piece, -1);
                next_nodes.emplace_back(new_piece);

                new_piece = piece;
                shift(new_piece, 1);
                next_nodes.emplace_back(new_piece);
            }
            open_nodes = next_nodes;
            next_nodes.clear();
        }
    }

    // shoutouts to friedkeenan
    // github link: https://github.com/friedkeenan/
    auto zero_g_starts = [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
        return std::array{
            valid_moves[Indices]...
        };
    }(std::make_index_sequence<zero_g_start_size[(size_t)p]>{});

    return zero_g_starts;
}

constexpr auto zero_g_starts_S = zero_g_starts<PieceType::S>();
constexpr auto zero_g_starts_Z = zero_g_starts<PieceType::Z>();
constexpr auto zero_g_starts_J = zero_g_starts<PieceType::J>();
constexpr auto zero_g_starts_L = zero_g_starts<PieceType::L>();
constexpr auto zero_g_starts_T = zero_g_starts<PieceType::T>();
constexpr auto zero_g_starts_O = zero_g_starts<PieceType::O>();
constexpr auto zero_g_starts_I = zero_g_starts<PieceType::I>();

namespace Shaktris {
    namespace MoveGen {
        namespace Traditional {
            // Movegen for a convex board with free movement at the top. (decided by board height of 16 or lower)
            inline std::vector<Piece> sky_piece_movegen(const Board& board, PieceType piece_type) {

                std::vector<Piece> valid_moves;

                switch (piece_type) {
                case PieceType::S:
                    valid_moves.assign(zero_g_starts_S.begin(), zero_g_starts_S.end());
                    break;
                case PieceType::Z:
                    valid_moves.assign(zero_g_starts_Z.begin(), zero_g_starts_Z.end());
                    break;
                case PieceType::J:
                    valid_moves.assign(zero_g_starts_J.begin(), zero_g_starts_J.end());
                    break;
                case PieceType::L:
                    valid_moves.assign(zero_g_starts_L.begin(), zero_g_starts_L.end());
                    break;
                case PieceType::T:
                    valid_moves.assign(zero_g_starts_T.begin(), zero_g_starts_T.end());
                    break;
                case PieceType::O:
                    valid_moves.assign(zero_g_starts_O.begin(), zero_g_starts_O.end());
                    break;
                case PieceType::I:
                    valid_moves.assign(zero_g_starts_I.begin(), zero_g_starts_I.end());
                    break;
                case PieceType::Empty:
                    break;
                case PieceType::PieceTypes_N:
                    break;
                    // default:
                        //break;
                }

                for (Piece& piece : valid_moves) {
                    Shaktris::Utility::sonic_drop(board, piece);
                }

                return valid_moves;
            }


            // movegen for if the board is convex and the height is 17 or higher
            inline std::vector<Piece> convex_movegen(const Board& board, PieceType piece_type) {
                Piece initial_piece = Piece(piece_type);

                std::vector<Piece> open_nodes;
                open_nodes.reserve(150);
                std::vector<Piece> next_nodes;
                next_nodes.reserve(150);
                std::bitset<6444> visited;

                std::vector<Piece> valid_moves;
                valid_moves.reserve(150);

                // rotations
                for (int r = 0; r < 4; r++) {
                    open_nodes.emplace_back(initial_piece);

                    initial_piece.rotate(TurnDirection::Right);

                    // lateral movement
                    while (open_nodes.size() > 0) {
                        // expand edges
                        for (auto& piece : open_nodes) {
                            size_t h = piece.compact_hash();
                            if (visited[h])
                                continue;
                            // mark node as visited
                            visited[h] = true;

                            valid_moves.push_back(piece);

                            Piece new_piece = piece;
                            Shaktris::Utility::shift(board, new_piece, -1);
                            next_nodes.emplace_back(new_piece);

                            new_piece = piece;
                            Shaktris::Utility::shift(board, new_piece, 1);
                            next_nodes.emplace_back(new_piece);
                        }
                        open_nodes = next_nodes;
                        next_nodes.clear();
                    }
                    if (initial_piece.type == PieceType::O && r == 0)
                        break;
                    if ((
                        initial_piece.type == PieceType::Z |
                        initial_piece.type == PieceType::S |
                        initial_piece.type == PieceType::O
                        ) && r == 0)
                        break;
                }

                for (Piece& piece : valid_moves) {
                    Shaktris::Utility::sonic_drop(board, piece);
                }

                return valid_moves;
            }


            // movegen for a non convex board with free movement at the top
            inline std::vector<Piece> movegen(const rotation_function rot_func, const Board& board, PieceType piece_type) {
                std::vector<Piece> valid_moves;

                bool convex = board.surface_convex();
                bool low = board.is_low();

                if (convex && low) {
                    valid_moves = sky_piece_movegen(board, piece_type);
                    return valid_moves;
                }
                else if (convex) {
                    valid_moves = convex_movegen(board, piece_type);
                    return valid_moves;
                }

                Piece initial_piece = Piece(piece_type);

                std::vector<Piece> open_nodes;
                open_nodes.reserve(150);

                if (low) {
                    open_nodes = sky_piece_movegen(board, piece_type);
                }
                else {
                    // root node
                    open_nodes.emplace_back(initial_piece);
                }

                std::vector<Piece> next_nodes;
                next_nodes.reserve(150);
                std::bitset<6444> visited;
                valid_moves.reserve(100);

                while (open_nodes.size() > 0) {
                    // expand edges
                    for (auto& piece : open_nodes) {
                        size_t h = piece.compact_hash();
                        if (visited[h])
                            continue;
                        // mark node as visited
                        visited[h] = true;

                        // try all movements
                        Piece new_piece = piece;
                        if (initial_piece.type != PieceType::O) {
                            rot_func(board, new_piece, TurnDirection::Left);
                            next_nodes.emplace_back(new_piece);

                            new_piece = piece;
                            rot_func(board, new_piece, TurnDirection::Right);
                            next_nodes.emplace_back(new_piece);
                        }

                        new_piece = piece;
                        Shaktris::Utility::shift(board, new_piece, -1);
                        next_nodes.emplace_back(new_piece);

                        new_piece = piece;
                        Shaktris::Utility::shift(board, new_piece, 1);
                        next_nodes.emplace_back(new_piece);

                        new_piece = piece;
                        Shaktris::Utility::sonic_drop(board, new_piece);
                        next_nodes.emplace_back(new_piece);

                        // check if the piece is grounded and therefore valid

                        piece.position.y--;

                        if (Shaktris::Utility::collides(board, piece)) {
                            piece.position.y++;
                            valid_moves.emplace_back(piece);
                        }
                    }
                    open_nodes = next_nodes;
                    next_nodes.clear();
                }

                return valid_moves;
            }

        }; // namespace Traditional

        namespace Smeared {


            column_t drop_column(column_t c) {
                return c | (c - (c & -c));
            }

            struct SmearedBoard {
                std::array<Board, 4> boards; // 0 => north, etc

                bool convex(bool surface) const {
                    bool ret = false;

                    if(surface) {
                        for (const auto& board : boards) {
                            ret |= !board.surface_convex();
                        }
                    }
                    else {
                        for (const auto& board : boards) {
                            ret |= !board.true_convex();
                        }
                    }

                    return !ret;
                bool operator==(const SmearedBoard& other) const {
                    return boards == other.boards; // std::array's == compares all elements
                }

                // shift both left and right
                inline SmearedBoard shift() const {
                    SmearedBoard ret = *this;
                    // shift each board to the direction
                    for (auto& board : ret.boards) {

                        board.board[1] |= board.board[0];
                        board.board[board.board.size() - 2] |= board.board[board.board.size() - 1];

                        for (size_t i = 1; i < board.board.size() - 1; ++i)
                        {
                            board.board[i - 1] |= board.board[i];
                            board.board[i + 1] |= board.board[i];
                        }
                    }

                    return ret;
                }

                inline bool empty() const {
                    bool has_mino = false;
                    for (const auto& board : boards) {
                        for (const auto& col : board.board) {
                            has_mino |= col != 0;
                        }
                    }
                    return !has_mino;
                }

                // the this is the board
                inline void non_collides(SmearedBoard& pieces)const {
                    //    A & ~B
                    // where A is the piece and B is this

                    for (size_t b_index = 0; b_index < boards.size(); ++b_index) {
                        const auto& board = boards[b_index];
                        const auto& piece = pieces.boards[b_index];
                        for (size_t x = 0; x < Board::width; x++) {
                            pieces.boards[b_index].board[x] = piece.board[x] & ~board.board[x];
                        }
                    }
                }

                inline void collides(SmearedBoard& pieces)const {

                    for (size_t b_index = 0; b_index < boards.size(); ++b_index) {
                        const auto& board = boards[b_index];
                        const auto& piece = pieces.boards[b_index];
                        for (size_t x = 0; x < Board::width; x++) {
                            pieces.boards[b_index].board[x] = piece.board[x] & board.board[x];
                        }
                    }
                }

                // the this is the board
                inline SmearedBoard rotate_srs(const SmearedBoard& pieces, PieceType type) const {

                    SmearedBoard ret;

                    const auto* offsets = &piece_offsets_JLSTZ;
                    const auto* prev_offsets = &piece_offsets_JLSTZ;

                    if (type == PieceType::I) {
                        offsets = &piece_offsets_I;
                        prev_offsets = &piece_offsets_I;
                    }
                    else if (type == PieceType::O) {
                        offsets = &piece_offsets_O;
                        prev_offsets = &piece_offsets_O;
                    }

                    SmearedBoard left_rotating_set = pieces;
                    SmearedBoard right_rotating_set = pieces;

                    left_rotating_set.rotate_left();
                    right_rotating_set.rotate_right();


                    std::array<Coord, 4> rot_offsets = { {{0, 0}, {0, 0}, {0, 0}, {0, 0}} };

                    // right srs
                    for (size_t srs_i = 0; srs_i < srs_kicks; srs_i++) {

                        rot_offsets[0].x = (*prev_offsets)[3][srs_i].x - (*offsets)[0][srs_i].x - rot_offsets[0].x;
                        rot_offsets[0].y = (*prev_offsets)[3][srs_i].y - (*offsets)[0][srs_i].y - rot_offsets[0].y;

                        rot_offsets[1].x = (*prev_offsets)[0][srs_i].x - (*offsets)[1][srs_i].x - rot_offsets[1].x;
                        rot_offsets[1].y = (*prev_offsets)[0][srs_i].y - (*offsets)[1][srs_i].y - rot_offsets[1].y;

                        rot_offsets[2].x = (*prev_offsets)[1][srs_i].x - (*offsets)[2][srs_i].x - rot_offsets[2].x;
                        rot_offsets[2].y = (*prev_offsets)[1][srs_i].y - (*offsets)[2][srs_i].y - rot_offsets[2].y;

                        rot_offsets[3].x = (*prev_offsets)[2][srs_i].x - (*offsets)[3][srs_i].x - rot_offsets[3].x;
                        rot_offsets[3].y = (*prev_offsets)[2][srs_i].y - (*offsets)[3][srs_i].y - rot_offsets[3].y;

                        if (!right_rotating_set.empty()) {
                            right_rotating_set.offset(rot_offsets);

                            ret |= right_rotating_set;

                            // get rid of pieces that didn't collide
                            this->collides(right_rotating_set);
                        }
                        else {
                            break;
                        }
                    }

                    rot_offsets = { {{0, 0}, {0, 0}, {0, 0}, {0, 0}} };


                    // left srs
                    for (size_t srs_i = 0; srs_i < srs_kicks; srs_i++) {

                        rot_offsets[0].x = (*prev_offsets)[1][srs_i].x - (*offsets)[0][srs_i].x - rot_offsets[0].x;
                        rot_offsets[0].y = (*prev_offsets)[1][srs_i].y - (*offsets)[0][srs_i].y - rot_offsets[0].y;

                        rot_offsets[1].x = (*prev_offsets)[2][srs_i].x - (*offsets)[1][srs_i].x - rot_offsets[1].x;
                        rot_offsets[1].y = (*prev_offsets)[2][srs_i].y - (*offsets)[1][srs_i].y - rot_offsets[1].y;

                        rot_offsets[2].x = (*prev_offsets)[3][srs_i].x - (*offsets)[2][srs_i].x - rot_offsets[2].x;
                        rot_offsets[2].y = (*prev_offsets)[3][srs_i].y - (*offsets)[2][srs_i].y - rot_offsets[2].y;

                        rot_offsets[3].x = (*prev_offsets)[0][srs_i].x - (*offsets)[3][srs_i].x - rot_offsets[3].x;
                        rot_offsets[3].y = (*prev_offsets)[0][srs_i].y - (*offsets)[3][srs_i].y - rot_offsets[3].y;

                        if (!left_rotating_set.empty()) {
                            left_rotating_set.offset(rot_offsets);

                            ret |= left_rotating_set;

                            // get rid of pieces that didn't collide
                            this->collides(left_rotating_set);
                        }
                        else {
                            break;
                        }
                    }

                    this->non_collides(ret);

                    return ret;
                }

                inline void rotate_right() {
                    if (boards.empty()) return; // Handle empty array
                    Board tmp = boards[boards.size() - 1];

                    // Shift elements right by 1 using std::memmove
                    std::memmove(&boards[1], &boards[0], (boards.size() - 1) * sizeof(Board));

                    boards[0] = tmp;
                }

                inline void rotate_left() {
                    if (boards.empty()) return; // Handle empty array
                    Board tmp = boards[0];

                    // Shift elements left by 1 using std::memmove
                    std::memmove(&boards[0], &boards[1], (boards.size() - 1) * sizeof(Board));

                    boards[boards.size() - 1] = tmp;
                }

                inline void offset_horizontal(int shift) {
                    constexpr size_t board_size = sizeof(uint32_t) * 10; // Size of one `Board`
                    constexpr size_t total_size = board_size * 4;        // Total size of `boards`

                    shift = (shift % 4 + 4) % 4; // Normalize shift to [0, 3]
                    if (shift == 0) return;     // No shift needed

                    alignas(32) std::array<std::array<uint32_t, 10>, 4> temp;

                    std::memcpy(temp.data(), boards.data(), shift * board_size);
                    std::memmove(boards.data(), boards.data() + shift, (4 - shift) * board_size);
                    std::memcpy(boards.data() + (4 - shift), temp.data(), shift * board_size);
                }

                inline void offset(const std::array<Coord, 4>& offsets) {

                    for (size_t rot = 0; rot < 4; ++rot) {
                        i8 dx = offsets[rot].x;
                        i8 dy = offsets[rot].y;
                        Board& current_smear = this->boards[rot];
                        Board tmp_board = boards[rot];

                        // shift the board up or down depending on the mino location (-y)
                        for (size_t i = 0; i < Board::width; ++i) {
                            auto& tmp_col = tmp_board.board[i];
                            auto real_col = boards[rot].board[i];
                            if (dy > 0)
                                real_col |= real_col << dy;
                            else if (dy < 0)
                                real_col |= real_col >> -dy;

                            tmp_col |= real_col;
                        }

                        offset_horizontal(dx);
                    }
                }

                // this is board
                inline SmearedBoard smear_drop(const SmearedBoard& pieces) const {
                    // pseudo code
                    // piece |= (piece >> 1) & ~column

                    SmearedBoard ret;

                    for (size_t b_index = 0; b_index < boards.size(); ++b_index) {
                        auto& board = this->boards[b_index];
                        auto& piece = pieces.boards[b_index];

                        for (size_t x = 0; x < Board::width; x++) {
                            auto piece_col = piece.board[x];
                            ret.boards[b_index].board[x] = drop_column(piece_col) & ~drop_column(board.board[x]);
                        }
                    }

                    return ret;
                }

                // this is board
                inline SmearedBoard grounded(const SmearedBoard& pieces) const {
                    // the way to check if a piece is grounded is to shift it down once
                    // and then check if it collides with the board
                    // if it does then its grounded
                    // but also all pieces that are on & 1 are grounded too

                    SmearedBoard ret;

                    for (size_t b_index = 0; b_index < boards.size(); ++b_index) {
                        auto& board = this->boards[b_index];
                        const auto& piece = pieces.boards[b_index];

                        for (size_t x = 0; x < Board::width; x++) {
                            auto piece_col = piece.board[x];
                            auto board_col = board.board[x];

                            piece_col >>= 1;
                            piece_col &= board_col;
                            piece_col <<= 1;
                            piece_col |= piece.board[x] & 1;

                            ret.boards[b_index].board[x] = piece_col;
                        }
                    }
                    return ret;
                }

                inline void operator|=(const SmearedBoard& other) {
                    for (size_t b_index = 0; b_index < boards.size(); ++b_index) {
                        auto& board = boards[b_index];
                        const auto& other_board = other.boards[b_index];
                        for (size_t x = 0; x < Board::width; x++) {
                            board.board[x] |= other_board.board[x];
                        }
                    }
                }

                inline void operator&=(const SmearedBoard& other) {
                    for (size_t b_index = 0; b_index < boards.size(); ++b_index) {
                        auto& board = boards[b_index];
                        const auto& other_board = other.boards[b_index];
                        for (size_t x = 0; x < Board::width; x++) {
                            board.board[x] &= other_board.board[x];
                        }
                    }
                }

                inline void operator^=(const SmearedBoard& other) {
                    for (size_t b_index = 0; b_index < boards.size(); ++b_index) {
                        auto& board = boards[b_index];
                        const auto& other_board = other.boards[b_index];
                        for (size_t x = 0; x < Board::width; x++) {
                            board.board[x] ^= other_board.board[x];
                        }
                    }
                }

                inline SmearedBoard operator^(const SmearedBoard& other) const {
                    SmearedBoard ret;

                    for (size_t b_index = 0; b_index < boards.size(); ++b_index) {
                        auto& board = boards[b_index];
                        const auto& other_board = other.boards[b_index];
                        for (size_t x = 0; x < Board::width; x++) {
                            ret.boards[b_index].board[x] = board.board[x] ^ other_board.board[x];
                        }
                    }

                    return ret;
                }
            };

            struct SmearedPiece {
                Coord position;
                u8 rot;
                u8 pad;
            };

            inline std::vector<Piece> moves_to_vec(const SmearedBoard& moves, PieceType type) {
                std::vector<Piece> ret;
                ret.reserve(150);
                for (size_t b_index = 0; b_index < moves.boards.size(); ++b_index) {
                    const auto& board = moves.boards[b_index];
                    for (size_t x = 0; x < Board::width; x++) {
                        auto col = moves.boards[b_index].board[x];
                        while (auto height = (sizeof(column_t) * CHAR_BIT) - std::countl_zero(col)) {
                            ret.emplace_back(type, (RotationDirection)b_index, Coord(x, height - 1));

                            col &= ~(1 << (height - 1)); // clear the bit
                        }
                    }
                }

                return ret;
            }

            inline std::vector<SmearedPiece> smeared_moves_to_vec(const SmearedBoard& moves, PieceType type) {
                std::vector<SmearedPiece> ret;
                ret.reserve(150);
                for (size_t b_index = 0; b_index < moves.boards.size(); ++b_index) {
                    const auto& board = moves.boards[b_index];
                    for (size_t x = 0; x < Board::width; x++) {
                        auto col = moves.boards[b_index].board[x];
                        while (auto height = (sizeof(column_t) * CHAR_BIT) - std::countl_zero(col)) {
                            ret.push_back(SmearedPiece{ Coord(x, height - 1), (u8)b_index });

                            col &= ~(1 << (height - 1)); // clear the bit
                        }
                    }
                }

                return ret;
            }

            inline SmearedBoard smear(const Board& board, PieceType type) {
                SmearedBoard ret{};
                std::array<column_t, 14> thick_board;
                // initialize the first and last two with ~0
                thick_board[0] = ~0;
                thick_board[1] = ~0;
                thick_board[12] = ~0;
                thick_board[13] = ~0;

                // initialize the middle with the board
                for (size_t i = 0; i < Board::width; i++) {
                    thick_board[i + 2] = board.board[i];
                }

                for (size_t rot = 0; rot < 4; rot++) {
                    for (const Coord& mino : rot_piece_def[static_cast<size_t>(type)][rot]) {
                        for (size_t x = 0; x < Board::width; x++) {
                            column_t c = thick_board[2 + x + mino.x];
                            bool cond = (mino.y < 0);
                            c = ((!cond) * (c >> mino.y)) | (cond * ~(~c << -mino.y));

                            ret.boards[rot].board[x] |= c;
                        }
                    }
                }

                return ret;
            }

            // movegen for only one rotation of the convex movegen
            inline Board partial_convex_movegen(const Board& board, const PieceType type) {
                Board ret{};
                for (size_t x = 0; x < Board::width; x++) {
                    // if column is nearly full skip
                    // (this is because the faster way of smearing does not set top bits because of the shift after the not)
                    // two because the I piece sticks out that long
                    bool cond = (board.board[x] >= std::numeric_limits<column_t>::max() >> 2);

                    auto height = (sizeof(column_t) * CHAR_BIT) - std::countl_zero(board.board[x]);
                    ret.board[x] = (!cond) * (1 << height);  // set the column to the height
                }

                return ret;
            }

            // Movegen for a convex board with free movement at the top. (decided by board height of 16 or lower)
            inline SmearedBoard convex_movegen(const Board& board, const PieceType type) {
                SmearedBoard ret;
                const SmearedBoard smeared_board = smear(board, type);
                for (size_t b_index = 0; b_index < smeared_board.boards.size(); ++b_index) {
                    const auto& s_board = smeared_board.boards[b_index];
                    ret.boards[b_index] = partial_convex_movegen(s_board, type);

                    if (type == PieceType::O && b_index == 0) {
                        break;
                    }
                    if ((
                        type == PieceType::I |
                        type == PieceType::Z |
                        type == PieceType::S
                        ) && b_index == 1) {
                        break;
                    }
                }

                return ret;
            }

            inline std::vector<Piece> movegen(const Board& board, PieceType type) {
                if (board.surface_convex()) {
                    return moves_to_vec(convex_movegen(board, type), type);
                }

                const SmearedBoard s_board = smear(board, type);

                SmearedBoard flood_old{};
                SmearedBoard flood_new{};

                flood_new.boards[0].board[4] = 1 << 19;

                while (flood_new != flood_old) {
                    flood_old = flood_new;

                    // down 
                    flood_new |= s_board.smear_drop(flood_new);

                    // left & right
                    flood_new |= flood_new.shift();

                    // rotate
                    flood_new |= s_board.rotate_srs(flood_new, type);

                    // cull
                    s_board.non_collides(flood_new);
                }
                // version of grounded() that doesn't require collision checking
                for (auto& board : flood_new.boards) {
                    for (size_t x = 0; x < Board::width; x++) {
                        // grounded pieces are last bits or bits followed by 0
                        board.board[x] = (board.board[x] & ~(board.board[x] << 1)) | (board.board[x] & 1);
                    }
                }
                return moves_to_vec(flood_new, type);
            }

            inline void srs(const SmearedBoard& s_board, SmearedPiece& p, PieceType type, TurnDirection dir) {
                const auto* offsets = &piece_offsets_JLSTZ;
                const auto* prev_offsets = &piece_offsets_JLSTZ;

                if (type == PieceType::I) {
                    offsets = &piece_offsets_I;
                    prev_offsets = &piece_offsets_I;
                }
                else if (type == PieceType::O) {
                    offsets = &piece_offsets_O;
                    prev_offsets = &piece_offsets_O;
                }

                u8 new_rot{};
                if (dir == TurnDirection::Right) {
                    new_rot = (p.rot + 1) % 4;
                } else {
                    new_rot = (p.rot + 3) % 4;
                }

                for (size_t i = 0; i < srs_kicks; ++i) {
                    const Coord offset = Coord(
                        (i8)(*prev_offsets)[p.rot][i].x - (i8)(*offsets)[static_cast<size_t>(new_rot)][i].x,
                        (i8)(*prev_offsets)[p.rot][i].y - (i8)(*offsets)[static_cast<size_t>(new_rot)][i].y);

                    SmearedPiece tmp = p;
                    tmp.position.x += offset.x;
                    tmp.position.y += offset.y;
                    tmp.rot = new_rot;

                    if (tmp.position.x >= 0 && tmp.position.x < Board::width && tmp.position.y >= 0) {
                        auto& col = s_board.boards[static_cast<size_t>(new_rot)].board[static_cast<size_t>(tmp.position.x)];
                        if (!(col & (column_t(1) << tmp.position.y))) {
                            p = tmp;
                            return;
                        }
                    }
                }
            }

            inline std::vector<Piece> god_movegen(const Board& board, const PieceType type) {
                std::vector<Piece> ret;
                ret.reserve(150);
                if (board.surface_convex() & board.is_low()) {
                    ret = moves_to_vec(convex_movegen(board, type), type);
                    return ret;
                }
                const SmearedBoard s_board = smear(board, type);
                std::vector<SmearedPiece> open_nodes;
                open_nodes.reserve(150);
                std::vector<SmearedPiece> next_nodes;
                next_nodes.reserve(150);
                std::bitset<32 * 10 * 4> visited;
                std::set<u32> hashes;
                auto is_immobile = [](const SmearedBoard& board, const SmearedPiece& piece) {
                    bool left = false;
                    bool right = false;
                    bool down = false;
                    bool up = false;
                    if (piece.position.x == 0)
                        left = true;
                    else {
                        const auto col = board.boards[static_cast<size_t>(piece.rot)].board[static_cast<size_t>(piece.position.x - 1)];
                        if (col & (1 << piece.position.y))
                            left = true;
                    }

                    if (piece.position.x == Board::width - 1)
                        right = true;
                    else {
                        const auto col = board.boards[static_cast<size_t>(piece.rot)].board[static_cast<size_t>(piece.position.x + 1)];
                        if (col & (1 << piece.position.y))
                            right = true;
                    }

                    if (piece.position.y == 0)
                        down = true;
                    else {
                        const auto col = board.boards[static_cast<size_t>(piece.rot)].board[static_cast<size_t>(piece.position.x)];
                        if (col & (1 << (piece.position.y - 1)))
                            down = true;
                    }

                    if (piece.position.y == Board::height - 1)
                        up = true;
                    else {
                        const auto col = board.boards[static_cast<size_t>(piece.rot)].board[static_cast<size_t>(piece.position.x)];
                        if (col & (1 << (piece.position.y + 1)))
                            up = true;
                    }

                    return left & right & down & up;
                };

                if (s_board.convex(PieceType::O == type)) {
                    SmearedBoard moves{};
                    for (size_t i = 0; i < 4; ++i) {
                        moves.boards[i] = partial_convex_movegen(s_board.boards[i], type);
                        if (type == PieceType::O && i == 0) {
                            break;
                        }
                        if ((
                                type == PieceType::I |
                                type == PieceType::Z |
                                type == PieceType::S) &&
                            i == 1) {
                            break;
                        }
                    }

                    ret = moves_to_vec(moves, type);
                    return ret;
                } else if (board.is_low()) {
                    SmearedBoard moves{};
                    for (u8 rot = 0; rot < 4; ++rot) {

                        moves.boards[rot] = partial_convex_movegen(s_board.boards[rot], type);

                        if (type == PieceType::O && rot == 0) {
                            break;
                        }
                        if ((
                                type == PieceType::I |
                                type == PieceType::Z |
                                type == PieceType::S) &&
                            rot == 1) {
                            break;
                        }
                    }
                    auto full_pieces = moves_to_vec(moves, type);

                    for(auto& piece : full_pieces) {
                        open_nodes.push_back(SmearedPiece{piece.position, (u8)piece.rotation});
                    }
                } else {
                    open_nodes.push_back({Coord((i8)4, (i8)19), 0});
                }

                while (!open_nodes.empty()) {
                    for (const auto& piece : open_nodes) {
                        auto to_iter = [&](auto x, auto y, auto r) {
                            return y + x * 32 + r * 32 * 10;
                        };
                        size_t iter = to_iter(piece.position.x, piece.position.y, piece.rot);

                        if (visited[iter])
                            continue;
                        visited[iter] = true;

                        // shift left
                        if (piece.position.x > 0) {
                            const auto col = s_board.boards[static_cast<size_t>(piece.rot)].board[static_cast<size_t>(piece.position.x - 1)];
                            if (!(col & (1 << piece.position.y)))
                                next_nodes.push_back({Coord(piece.position.x - 1, piece.position.y), piece.rot});
                        }

                        // shift right
                        if (piece.position.x < Board::width - 1) {
                            const auto col = s_board.boards[static_cast<size_t>(piece.rot)].board[static_cast<size_t>(piece.position.x + 1)];
                            if (!(col & (1 << piece.position.y))) {
                                next_nodes.push_back({Coord(piece.position.x + 1, piece.position.y), piece.rot});
                            }
                        }

                        // sonic drop
                        {
                            auto col = s_board.boards[static_cast<size_t>(piece.rot)].board[static_cast<size_t>(piece.position.x)];
                            // mask out every bit that is above the current y position
                            col &= (1 << piece.position.y) - 1;

                            const auto height = ((int)sizeof(column_t) * CHAR_BIT) - std::countl_zero(col);
                            // const auto height = std::clamp(piece.position.y - 1,0,32);
                            next_nodes.push_back({Coord(piece.position.x, height), piece.rot});
                        }

                        // rotate srs
                        if (type != PieceType::O) {
                            SmearedPiece next_piece = piece;

                            srs(s_board, next_piece, type, TurnDirection::Right);

                            next_nodes.push_back({next_piece});

                            next_piece = piece;

                            srs(s_board, next_piece, type, TurnDirection::Left);

                            next_nodes.push_back({next_piece});
                        }

                        // if is grounded push to ret
                        {
                            auto& col = s_board.boards[static_cast<size_t>(piece.rot)].board[static_cast<size_t>(piece.position.x)];
                            if ((piece.position.y == 0) || (col & (1 << (piece.position.y - 1)))) {
                                Piece p = Piece(type, (RotationDirection)piece.rot, piece.position, spinType::null);
                                u32 forward_hash = 0;
                                for (const auto& mino : p.minos) {
                                    forward_hash += p.position.x + mino.x;
                                    forward_hash *= 32;
                                    forward_hash += p.position.y + mino.y;
                                    forward_hash *= 10;
                                }

                                u32 backwards_hash = 0;
                                for (const auto& mino : std::views::reverse(p.minos)) {
                                    backwards_hash += p.position.x + mino.x;
                                    backwards_hash *= 32;
                                    backwards_hash += p.position.y + mino.y;
                                    backwards_hash *= 10;
                                }

                                forward_hash = std::min(forward_hash, backwards_hash);

                                if (!hashes.contains(forward_hash)) {
                                    hashes.insert(forward_hash);
                                    bool is_immobile_piece = is_immobile(s_board, piece);
                                    if (is_immobile_piece)
                                        p.spin = spinType::normal;
                                    ret.push_back(p);
                                }
                            }
                        }
                    }
                    std::swap(open_nodes, next_nodes);
                    next_nodes.clear();
                }

                // go through all pieces and check if they are grounded

                // check which pieces are immobile all spin


                return ret;
            }
        }; // namespace Smeared
    }; // namespace MoveGen
}; // namespace Shaktris

