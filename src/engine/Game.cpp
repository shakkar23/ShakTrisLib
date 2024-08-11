#include "Game.hpp"


#include <algorithm>
#include <bit>
#include <iostream>
#include <map>
#include <random>
#include <tuple>
#include <bitset>

#include "MoveGen.hpp"



void Game::place_piece() {
    board.set(current_piece);

    current_piece = queue.front();

    std::shift_left(queue.begin(), queue.end(), 1);

    queue.back() = PieceType::Empty;
}

void Game::do_hold() {

    if (hold) {
        PieceType current_piece_type = hold.value();
        hold = current_piece.type;
        current_piece = current_piece_type;
    }
    else {
        hold = current_piece.type;

        // shift queue
        current_piece = queue.front();

        std::shift_left(queue.begin(), queue.end(), 1);

        queue.back() = PieceType::Empty;
    }
}

bool Game::place_piece(const Piece& piece) {
    bool first_hold = false;
    if (piece.type != current_piece.type) {
        if (!hold.has_value())
        {  // shift queue
            std::shift_left(queue.begin(), queue.end(), 1);

            queue.back() = PieceType::Empty;

            first_hold = true;
        }
        hold = current_piece.type;
    }

    current_piece = piece;
    place_piece();

    return first_hold;
}

void Game::add_garbage(int lines, int location) {
    for (int i = 0; i < Board::width; ++i) {
        auto& column = board.board[i];

        column <<= lines;

        if (location != i) {
            column |= (1 << lines) - 1;
        }
    }
}



// helper type for the visitor #4
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

// ported from
// https://github.com/emmachase/tetrio-combo
int Game::damage_sent(int linesCleared, spinType spinType, bool pc) {


    return std::visit(overloaded{
            //[](auto arg) { /*very bad if this gets ran*/ },
            [&](TetrioS1& mode) { return mode.points(linesCleared,spinType, pc, combo, b2b); },
            [&](Botris& mode) { return mode.points(linesCleared,spinType, pc, combo, b2b); }
        }, mode);
}

void Game::process_movement(Piece& piece, Movement movement) const {
    switch (movement) {
        case Movement::Left:
            Shaktris::Utility::shift(board, piece, -1);
            break;
        case Movement::Right:
            Shaktris::Utility::shift(board, piece, 1);
            break;
        case Movement::RotateClockwise:
            srs_rotate(board, piece, TurnDirection::Right);
            break;
        case Movement::RotateCounterClockwise:
            srs_rotate(board, piece, TurnDirection::Left);
            break;
        case Movement::SonicDrop:
            Shaktris::Utility::sonic_drop(board, piece);
            break;
            // default:
            // std::unreachable();
    }
}



// warning! if there is a piece in the hold and the current piece is empty, we dont use the hold
std::vector<Piece> Game::get_possible_piece_placements() const {
    // we exausted the queue
    if (current_piece.type == PieceType::Empty)
        return {};

    std::vector<Piece> valid_pieces = Shaktris::MoveGen::Smeared::god_movegen(board, current_piece.type);

    PieceType holdType = hold.has_value() ? hold.value() : queue.front();
    if (holdType != PieceType::Empty && holdType != current_piece.type) {
        std::vector<Piece> hold_pieces = Shaktris::MoveGen::Smeared::god_movegen(board, holdType);
        valid_pieces.reserve(valid_pieces.size() + hold_pieces.size());
        for (auto& piece : hold_pieces) {
            valid_pieces.emplace_back(piece);
        }
    }

    return valid_pieces;
}