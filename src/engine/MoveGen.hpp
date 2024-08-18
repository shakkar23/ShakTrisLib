#pragma once

#include "ShaktrisConstants.hpp"
#include "Piece.hpp"
#include "Board.hpp"
#include "RotationSystems.hpp"

#include "Utility.hpp"


#include <array>
#include <cstdint>
#include <vector>
#include <bitset>
#include <bit>

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
    for (int r = 0; r < RotationDirections_N; r++) {
        open_nodes.emplace_back(initial_piece);
        if (initial_piece.type != PieceType::O)
            initial_piece.rotate(TurnDirection::Right);

        // lateral movement
        while (open_nodes.size() > 0) {
            // expand edges
            for (const auto& piece : open_nodes) {
                auto h = piece.compact_hash();
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
                auto h = piece.compact_hash();
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
                            auto h = piece.compact_hash();
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
                    if (initial_piece.type == PieceType::O)
                        break;
                }

                for (Piece& piece : valid_moves) {
                    Shaktris::Utility::sonic_drop(board, piece);
                }

                return valid_moves;
            }


            // movegen for a non convex board with free movement at the top
            inline std::vector<Piece> movegen(const rotation_function rot_func, const Board& board, PieceType piece_type) {

                bool convex = board.is_convex();
                bool low = board.is_low();

                if (convex && low) {
                    return sky_piece_movegen(board, piece_type);
                }
                else if (convex) {
                    return convex_movegen(board, piece_type);
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


                std::vector<Piece> valid_moves;
                valid_moves.reserve(100);



                while (open_nodes.size() > 0) {
                    // expand edges
                    for (auto& piece : open_nodes) {
                        auto h = piece.compact_hash();
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

            struct SmearedBoard {
                std::array<Board, 4> boards; // 0 => north, etc

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
                    bool ret = true;
                    for (const auto& board : boards) {
                        for (const auto& col : board.board) {
                            if (col != 0)
                                ret = false;
                        }
                    }
                    return ret;
                }

                // the this is the board
                inline void non_collides(SmearedBoard& pieces)const {
                    //    A & ~B
                    // where A is the piece and B is this

                    for (int b_index = 0; b_index < boards.size(); ++b_index) {
                        const auto& board = boards[b_index];
                        const auto& piece = pieces.boards[b_index];
                        for (int x = 0; x < Board::width; x++) {
                            pieces.boards[b_index].board[x] = piece.board[x] & ~board.board[x];
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

                    // for every piece offset
                    for (int srs_i = 0; srs_i < srs_kicks; srs_i++) {

                        std::array<Coord, 4> rot_offsets;
                        rot_offsets[0].x = (*prev_offsets)[3][srs_i].x - (*offsets)[0][srs_i].x;
                        rot_offsets[0].y = (*prev_offsets)[3][srs_i].y - (*offsets)[0][srs_i].y;

                        rot_offsets[1].x = (*prev_offsets)[0][srs_i].x - (*offsets)[1][srs_i].x;
                        rot_offsets[1].y = (*prev_offsets)[0][srs_i].y - (*offsets)[1][srs_i].y;

                        rot_offsets[2].x = (*prev_offsets)[1][srs_i].x - (*offsets)[2][srs_i].x;
                        rot_offsets[2].y = (*prev_offsets)[1][srs_i].y - (*offsets)[2][srs_i].y;

                        rot_offsets[3].x = (*prev_offsets)[2][srs_i].x - (*offsets)[3][srs_i].x;
                        rot_offsets[3].y = (*prev_offsets)[2][srs_i].y - (*offsets)[3][srs_i].y;

                        if (!right_rotating_set.empty()) {
                            SmearedBoard tmp_set = right_rotating_set;
                            tmp_set.rotate_right();
                            tmp_set.offset(rot_offsets);

                            // tmp after this line is which pieces did not collide after rotation
                            this->non_collides(tmp_set);

                            ret |= tmp_set;

                            // gotta rotate back to the original position
                            for (auto& offsets : rot_offsets) {
                                offsets.x *= -1;
                                offsets.y *= -1;
                            }

                            // tmp after this line is which pieces did not collide in the board space
                            tmp_set.offset(rot_offsets);
                            tmp_set.rotate_left();

                            // the ones that are still here are the ones we keep for the next iteration
                            right_rotating_set ^= tmp_set;
                        }

                        // use the rot_offsets for the other direction now
                        rot_offsets[0].x = (*prev_offsets)[1][srs_i].x - (*offsets)[0][srs_i].x;
                        rot_offsets[0].y = (*prev_offsets)[1][srs_i].y - (*offsets)[0][srs_i].y;

                        rot_offsets[1].x = (*prev_offsets)[2][srs_i].x - (*offsets)[1][srs_i].x;
                        rot_offsets[1].y = (*prev_offsets)[2][srs_i].y - (*offsets)[1][srs_i].y;

                        rot_offsets[2].x = (*prev_offsets)[3][srs_i].x - (*offsets)[2][srs_i].x;
                        rot_offsets[2].y = (*prev_offsets)[3][srs_i].y - (*offsets)[2][srs_i].y;

                        rot_offsets[3].x = (*prev_offsets)[0][srs_i].x - (*offsets)[3][srs_i].x;
                        rot_offsets[3].y = (*prev_offsets)[0][srs_i].y - (*offsets)[3][srs_i].y;

                        if (!left_rotating_set.empty()) {
                            SmearedBoard tmp_set = left_rotating_set;
                            tmp_set.rotate_left();
                            tmp_set.offset(rot_offsets);

                            // tmp after this line is which pieces did not collide after rotation
                            this->non_collides(tmp_set);

                            ret |= tmp_set;
                            // figure out which pieces do not collide by bringing them back to the original position and check if they are in the rotating pieces

                            for (auto& offsets : rot_offsets) {
                                offsets.x *= -1;
                                offsets.y *= -1;
                            }

                            // tmp after this line is which pieces did not collide in the board space
                            tmp_set.offset(rot_offsets);
                            tmp_set.rotate_right();

                            // the ones that are still here are the ones we keep for the next iteration
                            left_rotating_set ^= tmp_set;
                        }
                    }

                    // add the remaining pieces that did not rotate :(
                    ret |= left_rotating_set;
                    ret |= right_rotating_set;
                    return ret;
                }

                inline void rotate_right() {
                    Board tmp = boards[boards.size() - 1];

                    for (int b_index = boards.size() - 1; b_index > 0; --b_index) {
                        boards[b_index] = boards[b_index - 1];
                    }
                    boards[0] = tmp;
                }

                inline void rotate_left() {
                    Board tmp = boards[0];

                    for (int b_index = 0; b_index < boards.size() - 1; ++b_index) {
                        boards[b_index] = boards[b_index + 1];
                    }
                    boards[boards.size() - 1] = tmp;
                }

                inline void offset(const std::array<Coord, 4>& offsets) {

                    for (int rot = 0; rot < 4; ++rot) {
                        i8 dx = offsets[rot].x;
                        i8 dy = offsets[rot].y;
                        Board& current_smear = this->boards[rot];
                        Board tmp_board = boards[rot];

                        // shift the board up or down depending on the mino location (-y)
                        for (int i = 0; i < Board::width; ++i) {
                            auto& tmp_col = tmp_board.board[i];
                            auto real_col = boards[rot].board[i];
                            if (dy > 0)
                                real_col |= real_col << dy;
                            else if (dy < 0)
                                real_col |= real_col >> -dy;

                            tmp_col |= real_col;
                        }

                        for (int x = 1; x < Board::width - 1; ++x) {
                            auto& smear_col = current_smear.board[x];
                            auto& tmp_col = tmp_board.board[x + dx];

                            smear_col = tmp_col;
                        }
                    }
                }

                // this is board
                inline SmearedBoard smear_drop(const SmearedBoard& pieces) const {
                    // pseudo code
                    // piece |= (piece >> 1) & ~column

                    SmearedBoard ret;

                    for (int b_index = 0; b_index < boards.size(); ++b_index) {
                        auto& board = this->boards[b_index];
                        auto& piece = pieces.boards[b_index];

                        for (int x = 0; x < Board::width; x++) {
                            auto piece_col = piece.board[x];
                            for (int n = 0; n < 32; n++) {
                                piece_col |= (piece_col >> 1) & ~board.board[x];
                            }
                            ret.boards[b_index].board[x] = piece_col;
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

                    for (int b_index = 0; b_index < boards.size(); ++b_index) {
                        auto& board = this->boards[b_index];
                        const auto& piece = pieces.boards[b_index];

                        for (int x = 0; x < Board::width; x++) {
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
                    for (int b_index = 0; b_index < boards.size(); ++b_index) {
                        auto& board = boards[b_index];
                        const auto& other_board = other.boards[b_index];
                        for (int x = 0; x < Board::width; x++) {
                            board.board[x] |= other_board.board[x];
                        }
                    }
                }

                inline void operator&=(const SmearedBoard& other) {
                    for (int b_index = 0; b_index < boards.size(); ++b_index) {
                        auto& board = boards[b_index];
                        const auto& other_board = other.boards[b_index];
                        for (int x = 0; x < Board::width; x++) {
                            board.board[x] &= other_board.board[x];
                        }
                    }
                }

                inline void operator^=(const SmearedBoard& other) {
                    for (int b_index = 0; b_index < boards.size(); ++b_index) {
                        auto& board = boards[b_index];
                        const auto& other_board = other.boards[b_index];
                        for (int x = 0; x < Board::width; x++) {
                            board.board[x] ^= other_board.board[x];
                        }
                    }
                }

                inline SmearedBoard operator^(const SmearedBoard& other) const {
                    SmearedBoard ret;

                    for (int b_index = 0; b_index < boards.size(); ++b_index) {
                        auto& board = boards[b_index];
                        const auto& other_board = other.boards[b_index];
                        for (int x = 0; x < Board::width; x++) {
                            ret.boards[b_index].board[x] = board.board[x] ^ other_board.board[x];
                        }
                    }

                    return ret;
                }
            };

            struct SmearedPiece {
                Coord position;
                u8 rot;
            };

            inline std::vector<Piece> moves_to_vec(const SmearedBoard& moves, PieceType type) {
                std::vector<Piece> ret;
                ret.reserve(150);
                for (int b_index = 0; b_index < moves.boards.size(); ++b_index) {
                    const auto& board = moves.boards[b_index];
                    for (int x = 0; x < Board::width; x++) {
                        auto col = moves.boards[b_index].board[x];
                        while (auto height = (sizeof(column_t) * CHAR_BIT) - std::countl_zero(col)) {
                            ret.emplace_back(type, (RotationDirection)b_index, Coord(x, height - 1));

                            col &= ~(1 << (height - 1)); // clear the bit
                        }
                    }
                }

                return ret;
            }

            inline SmearedBoard smear(const Board& board, PieceType type) {
                SmearedBoard ret{};

                for (int rot = 0; rot < 4; rot++) {
                    for (const Coord& mino : rot_piece_def[static_cast<size_t>(rot)][static_cast<size_t>(type)]) {
                        for (int x = 0; x < Board::width; x++) {
                            auto c = ((x + mino.x) >= 0 && (x + mino.x) < board.board.size()) ? board.board[x + mino.x] : column_t(~0);
                            bool cond = (mino.y < 0);
                            c = ((!cond) * (c >> mino.y)) | (cond * ~(~c << -mino.y));

                            ret.boards[rot].board[x] |= c;
                        }
                    }
                }

                return ret;
            }

            // Movegen for a convex board with free movement at the top. (decided by board height of 16 or lower)
            inline SmearedBoard convex_movegen(const Board& board, PieceType type) {
                SmearedBoard ret{};
                const SmearedBoard smeared_board = smear(board, type);
                for (int b_index = 0; b_index < smeared_board.boards.size(); ++b_index) {
                    const auto& s_board = smeared_board.boards[b_index];
                    for (int x = 0; x < Board::width; x++) {
                        // if column is nearly full skip 
                        // (this is because the faster way of smearing does not set top bits because of the shift after the not)
                        // two because the I piece sticks out that long
                        bool cond = (s_board.board[x] >= std::numeric_limits<column_t>::max() >> 2);

                        auto height = (sizeof(column_t) * CHAR_BIT) - std::countl_zero(s_board.board[x]);
                        ret.boards[b_index].board[x] = (!cond) * (1 << height); // set the column to the height
                    }
                    if (type == PieceType::O) {
                        break;
                    }
                }

                return ret;
            }

            inline std::vector<Piece> movegen(const Board& board, PieceType type) {
                if (board.is_convex()) {
                    return moves_to_vec(convex_movegen(board, type), type);
                }
                const SmearedBoard smeared_board = smear(board, type);
                SmearedBoard visited{};
                SmearedBoard open_nodes{};
                SmearedBoard next_nodes{};

                open_nodes.boards[0].board[4/* x */] = 1 << 19/* y */; // set north piece rotation to the piece position

                for (int i = 0; i < 5; ++i) { // valid shifts put onto the next nodes
                    open_nodes = open_nodes.shift();
                    smeared_board.non_collides(open_nodes);
                }
                while (!open_nodes.empty()) {

                    { // valid shifts put onto the next nodes
                        SmearedBoard shifted_pieces = open_nodes.shift();
                        smeared_board.non_collides(shifted_pieces);
                        visited.non_collides(shifted_pieces);
                        visited |= shifted_pieces;
                        next_nodes |= shifted_pieces;
                    }

                    { // valid rotations put onto the next nodes
                        SmearedBoard tmp_pieces = smeared_board.rotate_srs(open_nodes, type);
                        smeared_board.non_collides(tmp_pieces);
                        visited.non_collides(tmp_pieces);
                        visited |= tmp_pieces;
                        next_nodes |= tmp_pieces;
                    }

                    { // already taken care of by smear drop
                        SmearedBoard smeared_pieces = smeared_board.smear_drop(open_nodes);
                        // smeared_board.non_collides(smeared_pieces);
                        visited.non_collides(smeared_pieces);
                        visited |= smeared_pieces;
                        next_nodes |= smeared_pieces;
                    }

                    std::swap(open_nodes, next_nodes);
                    next_nodes = {};
                }

                return moves_to_vec(smeared_board.grounded(visited), type);
            }


            inline void srs(const SmearedBoard& s_board, SmearedPiece& p, PieceType type, TurnDirection dir) {
                u8 prev_rot = p.rot;

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


                if (dir == TurnDirection::Right) {
                    p.rot = (p.rot + 1) % 4;
                }
                else {
                    p.rot = (p.rot + 3) % 4;
                }

                for (int i = 0; i < srs_kicks; ++i) {
                    const Coord offset = Coord(
                        (i8)(*prev_offsets)[prev_rot][i].x - (i8)(*offsets)[p.rot][i].x,
                        (i8)(*prev_offsets)[prev_rot][i].y - (i8)(*offsets)[p.rot][i].y
                    );

                    SmearedPiece tmp = p;
                    tmp.position.x += offset.x;
                    tmp.position.y += offset.y;

                    if (tmp.position.x >= 0 && tmp.position.x < Board::width) {
                        if (tmp.position.y >= 0) {
                            auto& col = s_board.boards[p.rot].board[tmp.position.x];
                            if (!(col & (1 << tmp.position.y))) {
                                p = tmp;
                                return;
                            }
                        }
                    }
                }

                p.rot = prev_rot;

            }

            inline std::vector<Piece> god_movegen(const Board& board, PieceType type) {
                if (board.is_convex()) {
                    return moves_to_vec(convex_movegen(board, type), type);
                }
                const SmearedBoard s_board = smear(board, type);
                std::vector<SmearedPiece> open_nodes; open_nodes.reserve(200);
                std::vector<SmearedPiece> next_nodes; next_nodes.reserve(200);
                std::vector<Piece> ret; ret.reserve(200);
                std::bitset<32 * 10 * 4> visited;
                auto to_iter = [&](u8 x, u8 y, u8 r) {
                    return r + x * 4 + y * 4 * 10;
                };
                auto is_immobile = [](const SmearedBoard& board, const SmearedPiece& piece) {
                    bool left = false;
                    bool right = false;
                    bool down = false;
                    bool up = false;
                    if (piece.position.x == 0)
                    {
                        left = true;
                    }
                    else
                    {
                        const auto col = board.boards[piece.rot].board[piece.position.x - 1];
                        if (col & (1 << piece.position.y))
                        {
                            left = true;
                        }
                    }

                    if (piece.position.x == Board::width - 1)
                    {
                        right = true;
                    }
                    else
                    {
                        const auto col = board.boards[piece.rot].board[piece.position.x + 1];
                        if (col & (1 << piece.position.y))
                        {
                            right = true;
                        }
                    }
                    
                    if (piece.position.y == 0)
					{
						down = true;
					}
                    else
                    {
                        const auto col = board.boards[piece.rot].board[piece.position.x];
                        if (col & (1 << (piece.position.y - 1)))
                        {
                            down = true;
                        }
                    }

                    if (piece.position.y == Board::height - 1)
                    {
                        up = true;
					}
					else
					{
						const auto col = board.boards[piece.rot].board[piece.position.x];
						if (col & (1 << (piece.position.y + 1)))
						{
							up = true;
						}
                    }

                    return left && right && down && up;
                    };

                open_nodes.push_back({ Coord(4, 19), 0 });

                while (!open_nodes.empty()) {
                    for (const auto& piece : open_nodes) {

                        auto iter = to_iter(piece.position.x, piece.position.y, piece.rot);


                        if(visited[iter])
							continue;
                        visited[iter] = true;

                        // shift left
                        if (piece.position.x > 0) {
                            const auto col = s_board.boards[piece.rot].board[piece.position.x - 1];
                            if (!(col & (1 << piece.position.y)))
                                next_nodes.push_back({ Coord(piece.position.x - 1, piece.position.y), piece.rot });
                        }
                        // shift right
                        if (piece.position.x < Board::width - 1) {
                            const auto col = s_board.boards[piece.rot].board[piece.position.x + 1];
                            if (!(col & (1 << piece.position.y))) {
                                next_nodes.push_back({ Coord(piece.position.x + 1, piece.position.y), piece.rot });
                            }
                        }

                        // sonic drop
                        {
                            auto col = s_board.boards[piece.rot].board[piece.position.x];
                            // mask out every bit that is above the current y position
                            col &= (1 << piece.position.y) - 1;
                            
                            const auto height = ((int)sizeof(column_t) * CHAR_BIT) - std::countl_zero(col);
                            next_nodes.push_back({ Coord(piece.position.x, height), piece.rot });
                        }

                        // rotate srs
                        if(type != PieceType::O) {
                            SmearedPiece next_piece = piece;
                             
                            srs(s_board, next_piece, type, TurnDirection::Right);

                            next_nodes.push_back({ next_piece });


                            next_piece = piece;

                            srs(s_board, next_piece, type, TurnDirection::Left);

                            next_nodes.push_back({ next_piece });
                        }

                        // if is grounded push to ret
						{
							auto &col = s_board.boards[piece.rot].board[piece.position.x];
                            if (piece.position.y == 0 || col & (1 << (piece.position.y - 1))) {
                                bool is_immobile_piece = is_immobile(s_board, piece);
                                Piece p = Piece(type, (RotationDirection)piece.rot, piece.position, is_immobile_piece ? spinType::normal : spinType::null);
								ret.push_back(p);
							}
						}
                    }
                    open_nodes = next_nodes;
                    next_nodes.clear();
                }

                // go through all pieces and check if they are grounded

                // check which pieces are immobile all spin


                return ret;

            }
        }; // namespace Smeared
    }; // namespace MoveGen
}; // namespace Shaktris

