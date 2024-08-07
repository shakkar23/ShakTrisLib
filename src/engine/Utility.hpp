#pragma once

#include "ShaktrisConstants.hpp"
#include "Board.hpp"
#include "Piece.hpp"

#include <algorithm>
#include <bit>

// utilities for moving pieces
namespace Shaktris {

    namespace Utility {


        inline bool collides(const Board& board, const Piece& piece) {
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

        inline void shift(const Board& board, Piece& piece, int dir) {
            piece.position.x += dir;

            if (collides(board, piece))
                piece.position.x -= dir;
            else
                piece.spin = spinType::null;
        }

        inline void sonic_drop(const Board board, Piece& piece) {
            int distance = sizeof(column_t) * CHAR_BIT;
            for (auto& mino : piece.minos) {

                int mino_height = mino.y + piece.position.y;

                uint32_t column = board.board[mino.x + piece.position.x];

                if (column && mino_height != 0) {
                    int air = 32 - mino_height;
                    mino_height -= 32 - std::countl_zero((column << air) >> air);
                }

                distance = std::min(distance, mino_height);
            }

            piece.position.y -= distance;
        }

        inline int get_garbage_height(const Board& board) {

            int max_air = -1;

            for (int i = 0; i < Board::width; ++i) {
                auto& col = board.board[i];
                int air = std::countl_zero(col);
                max_air = std::max(air, max_air);
            }

            return 32 - max_air;
        }
    };
};
