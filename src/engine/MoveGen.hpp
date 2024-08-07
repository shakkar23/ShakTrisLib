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

        // movegen functions

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
        inline std::vector<Piece> movegen(rotation_function rot_func, const Board& board, PieceType piece_type) {

            auto is_convex = [](const Board& board) {
                Board shifted_board;

                int garbage_height = Shaktris::Utility::get_garbage_height(board);

                bool convex = true;

                for (int i = 0; i < Board::width; ++i) {
                    shifted_board.board[i] = board.board[i] >> garbage_height;
                }

                for (int i = 0; i < Board::width; ++i) {
                    auto& col = shifted_board.board[i];
                    convex = convex && (std::popcount(col) == std::countr_one(col));
                }

                return convex;

                };
            auto is_low = [](const Board& board) {
                constexpr auto p = Piece(PieceType::I);

                constexpr uint32_t high_collider = ~((1 << (p.position.y - 2)) - 1);

                bool ret = true;

                for (size_t x = 0; x < Board::width; x++) {
                    if (board.get_column(x) & high_collider)
                        ret = false;
                }
                return ret;
                };
            bool convex = is_convex(board);
            bool low = is_low(board);

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


    };
};
