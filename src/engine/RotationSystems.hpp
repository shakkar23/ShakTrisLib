#pragma once

#include "ShaktrisConstants.hpp"
#include "Utility.hpp"

#include <array>
#include <cstdint>

constexpr std::size_t srs_kicks = 5;

constexpr std::array<std::array<Coord, srs_kicks>, RotationDirections_N> piece_offsets_JLSTZ = { {
    {{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}},
    {{{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}}},
    {{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}},
    {{{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}},
} };

constexpr std::array<std::array<Coord, srs_kicks>, RotationDirections_N> piece_offsets_O = { {
    {{{0, 0}}},
    {{{0, -1}}},
    {{{-1, -1}}},
    {{{-1, 0}}},
} };

constexpr std::array<std::array<Coord, srs_kicks>, RotationDirections_N> piece_offsets_I = { {

    {{{0, 0}, {-1, 0}, {2, 0}, {-1, 0}, {2, 0}}},
    {{{-1, 0}, {0, 0}, {0, 0}, {0, 1}, {0, -2}}},
    {{{-1, 1}, {1, 1}, {-2, 1}, {1, 0}, {-2, 0}}},
    {{{0, 1}, {0, 1}, {0, 1}, {0, -1}, {0, 2}}},
} };

using rotation_function = void (*)(const Board&, Piece&, TurnDirection);

inline void srs_rotate(const Board& board,Piece& piece, TurnDirection dir) {
    const RotationDirection prev_rot = piece.rotation;

    piece.rotate(dir);

    const std::array<Coord, srs_kicks>* offsets = &piece_offsets_JLSTZ[piece.rotation];
    const std::array<Coord, srs_kicks>* prev_offsets = &piece_offsets_JLSTZ[prev_rot];

    if (piece.type == PieceType::I) {
        prev_offsets = &piece_offsets_I[prev_rot];
        offsets = &piece_offsets_I[piece.rotation];
    }
    else if (piece.type == PieceType::O) {
        prev_offsets = &piece_offsets_O[prev_rot];
        offsets = &piece_offsets_O[piece.rotation];
    }

    auto x = piece.position.x;
    auto y = piece.position.y;

    for (int i = 0; i < srs_kicks; i++) {
        piece.position.x = x + (*prev_offsets)[i].x - (*offsets)[i].x;
        piece.position.y = y + (*prev_offsets)[i].y - (*offsets)[i].y;
        if (!Shaktris::Utility::collides(board, piece)) {
            if (piece.type == PieceType::T) {
                constexpr std::array<std::array<Coord, 4>, 4> corners = { {
                        // a       b       c        d
                        {{{-1,  1}, { 1,  1}, { 1, -1}, {-1, -1}}},  // North
                        {{{ 1,  1}, { 1, -1}, {-1, -1}, {-1,  1}}},  // East
                        {{{ 1, -1}, {-1, -1}, {-1,  1}, { 1,  1}}},  // South
                        {{{-1, -1}, {-1,  1}, { 1,  1}, { 1, -1}}},  // West
                    } };

                bool filled[4] = { true, true, true, true };

                for (int u = 0; u < 4; u++) {
                    Coord c = corners[piece.rotation][u];
                    c.x += piece.position.x;
                    c.y += piece.position.y;
                    if (c.x >= 0 && c.x < Board::width)
                        filled[u] = board.get(c.x, c.y);
                }

                if (filled[0] && filled[1] && (filled[2] || filled[3]))
                    piece.spin = spinType::normal;
                else if ((filled[0] || filled[1]) && filled[2] && filled[3]) {
                    if (i >= (srs_kicks - 1)) {
                        piece.spin = spinType::normal;
                    }
                    else
                        piece.spin = spinType::mini;
                }
                else
                    piece.spin = spinType::null;
            }
            return;
        }
    }

    piece.position.x = x;
    piece.position.y = y;

    piece.rotate(dir == TurnDirection::Left ? TurnDirection::Right : TurnDirection::Left);
}



inline void srs_all_spin_rotate(const Board& board, Piece& piece, TurnDirection dir) {
    const RotationDirection prev_rot = piece.rotation;

    piece.rotate(dir);

    const std::array<Coord, srs_kicks>* offsets = &piece_offsets_JLSTZ[piece.rotation];
    const std::array<Coord, srs_kicks>* prev_offsets = &piece_offsets_JLSTZ[prev_rot];

    if (piece.type == PieceType::I) {
        prev_offsets = &piece_offsets_I[prev_rot];
        offsets = &piece_offsets_I[piece.rotation];
    }
    else if (piece.type == PieceType::O) {
        prev_offsets = &piece_offsets_O[prev_rot];
        offsets = &piece_offsets_O[piece.rotation];
    }

    auto x = piece.position.x;
    auto y = piece.position.y;

    for (int i = 0; i < srs_kicks; i++) {
        piece.position.x = x + (*prev_offsets)[i].x - (*offsets)[i].x;
        piece.position.y = y + (*prev_offsets)[i].y - (*offsets)[i].y;
        if (!Shaktris::Utility::collides(board, piece)) {
            piece.spin = spinType::null;

            piece.position.x += 1;
            if (!Shaktris::Utility::collides(board, piece)) {
                piece.position.x -= 1;
                return;
            }
            piece.position.x -= 2;
            if (!Shaktris::Utility::collides(board, piece)) {
                piece.position.x += 1;
                return;
            }
            piece.position.x += 1;
            piece.position.y += 1;
            if (!Shaktris::Utility::collides(board, piece)) {
                piece.position.y -= 1;
                return;
            }
            piece.position.y -= 2;
            if (!Shaktris::Utility::collides(board, piece)) {
                piece.position.y += 1;
                return;
            }
            piece.position.y += 1;

            piece.spin = spinType::normal;
            return;
        }
    }

    piece.position.x = x;
    piece.position.y = y;

    piece.rotate(dir == TurnDirection::Left ? TurnDirection::Right : TurnDirection::Left);
}

