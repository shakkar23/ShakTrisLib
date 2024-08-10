#pragma once

#include "ShaktrisConstants.hpp"
#include "Board.hpp"
#include "Piece.hpp"

#include <algorithm>
#include <bit>

// utilities for moving pieces
namespace Shaktris {

    namespace Utility {

        constexpr inline bool collides(const Board& board, const Piece& piece) {
            for (auto& mino : piece.minos) {
                int x_pos = mino.x + piece.position.x;
                if (x_pos < 0 || x_pos >= Board::width)
                    return true;

                int y_pos = mino.y + piece.position.y;
                if (y_pos < 0 || y_pos >= Board::height)
                    return true;
                if (board.get(x_pos, y_pos))
                    return true;
            }

            return false;
        }

        constexpr inline void shift(const Board& board, Piece& piece, int dir) {
            piece.position.x += dir;

            if (collides(board, piece))
                piece.position.x -= dir;
            else
                piece.spin = spinType::null;
        }

        constexpr inline void sonic_drop(const Board &board, Piece& piece) {
            std::array<i8, 4> mino_heights;
            for (size_t i = 0; i < 4; ++i) {
                auto mino = piece.minos[i];
                i8 mino_height = mino.y + piece.position.y;

                const auto column = board.board[mino.x + piece.position.x];

                if (column && mino_height) {
                    mino_height -= 32 - std::countl_zero(column & ((1 << mino_height) - 1));
                }

                mino_heights[i] = mino_height;
            }
            i8 dist = mino_heights[0];
            for(int i  = 1; i < 4; ++i) {
                dist = std::min(dist, mino_heights[i]);
			}
            piece.position.y -= dist;
        }

        
    };
};
