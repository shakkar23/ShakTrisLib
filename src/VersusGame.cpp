#include "VersusGame.hpp"

#include <algorithm>
#include <set>

#include "Move.hpp"
#include "util/rng.hpp"
#include "engine/MoveGen.hpp"

void VersusGame::play_moves() {
    // game over conditions

    if (game_over) {
        return;
    }

    if (p1_move.null_move && p2_move.null_move) {
        p1_move.null_move = false;
        p2_move.null_move = false;
    }

    turn += 1;

    int p1_cleared_lines = 0;
    bool p1_first_hold = false;
    // player 1 move
    if (!p1_move.null_move) {
        spinType spin = p1_move.piece.spin;

        p1_first_hold = p1_game.place_piece(p1_move.piece);
        p1_cleared_lines = p1_game.board.clearLines();

        bool pc = true;
        for (int i = 0; i < Board::width; i++) {
            if (p1_game.board.get_column(i) != 0) {
                pc = false;
                break;
            }
        }

        if (spin == spinType::normal && p1_cleared_lines > 0) {
            // std::cout << "p1 did T-spin " << p1_cleared_lines << std::endl;
        }

        if (pc) {
            // std::cout << "p1 did perfect clear" << std::endl;
        }

        int dmg = p1_game.damage_sent(p1_cleared_lines, spin, pc);

        p1_atk += dmg;

        p2_meter += dmg;
    }

    int p2_cleared_lines = 0;
    bool p2_first_hold = false;
    // player 1 move
    if (!p2_move.null_move) {
        spinType spin = p2_move.piece.spin;

        p2_first_hold = p2_game.place_piece(p2_move.piece);
        p2_cleared_lines = p2_game.board.clearLines();

        bool pc = true;
        for (int i = 0; i < Board::width; i++) {
            if (p2_game.board.get_column(i) != 0) {
                pc = false;
                break;
            }
        }

        if (spin == spinType::normal && p2_cleared_lines > 0) {
            // std::cout << "p2 did T-spin " << p2_cleared_lines << std::endl;
        }

        if (pc) {
            // std::cout << "p2 did perfect clear" << std::endl;
        }

        int dmg = p2_game.damage_sent(p2_cleared_lines, spin, pc);

        p2_atk += dmg;

        p1_meter += dmg;
    }

    int min_meter = std::min(p1_meter, p2_meter);

    p1_meter -= min_meter;
    p2_meter -= min_meter;

    // if possible for both players to have damage, update here
    if (!p1_move.null_move && p1_cleared_lines == 0) {
        if (p1_meter > 0) {
            // player 2 accepts garbage!
            p1_game.add_garbage(p1_meter, p1_rng.getRand(Board::width));
            p1_meter = 0;
        }
    }

    if (!p2_move.null_move && p2_cleared_lines == 0) {
        if (p2_meter > 0) {
            // player 2 accepts garbage!
            p2_game.add_garbage(p2_meter, p2_rng.getRand(Board::width));
            p2_meter = 0;
        }
    }

    if (p1_first_hold)
        *(p1_game.queue.end() - 2) = p1_rng.getPiece();

    if (p2_first_hold)
        *(p2_game.queue.end() - 2) = p2_rng.getPiece();

    p1_game.queue.back() = p1_rng.getPiece();
    p2_game.queue.back() = p2_rng.getPiece();

    if (Shaktris::Utility::collides(p1_game.board, p1_game.current_piece)) {
        game_over = true;
    }

    if (Shaktris::Utility::collides(p2_game.board, p2_game.current_piece)) {
        game_over = true;
    }
}

VersusGame VersusGame::play_moves_not_inplace() {
    VersusGame copy = VersusGame(*this);

    copy.play_moves();

    return copy;
}

std::vector<Move> VersusGame::get_moves(int id) const {
    std::vector<Move> moves;

    const Game& player = id == 0 ? p1_game : p2_game;

    auto movegen_pieces = Shaktris::MoveGen::Traditional::movegen(srs_rotate, player.board, player.current_piece.type);

    PieceType hold = player.hold.has_value() ? player.hold.value() : player.queue.front();

    std::vector<Piece> hold_pieces;
    if (hold != PieceType::Empty)
        hold_pieces = Shaktris::MoveGen::Traditional::movegen(srs_rotate, player.board, hold);

    moves.reserve((movegen_pieces.size() + hold_pieces.size()) * 2);

    for (auto& piece : movegen_pieces) {
        moves.emplace_back(piece, false);
        moves.emplace_back(piece, true);
    }

    for (auto& piece : hold_pieces) {
        moves.emplace_back(piece, false);
        moves.emplace_back(piece, true);
    }

    return moves;
}

std::vector<Move> VersusGame::get_N_moves(int id, int N) const {
    const Game& player = id == 0 ? p1_game : p2_game;
    std::vector<Piece> valid_pieces = Shaktris::MoveGen::Traditional::movegen(srs_rotate, player.board, player.current_piece.type);
    PieceType holdType = player.hold.has_value() ? player.hold.value() : player.queue.front();

    std::vector<Piece> hold_pieces = Shaktris::MoveGen::Traditional::movegen(srs_rotate, player.board, holdType);
    valid_pieces.reserve(valid_pieces.size() + hold_pieces.size());
    for (auto& piece : hold_pieces) {
        valid_pieces.emplace_back(piece);
    }

    std::vector<std::optional<Piece>> moves;

    for (auto& piece : valid_pieces) {
        Board temp_board = player.board;
        temp_board.set(piece);
        moves.emplace_back(piece);
    }

    // try the null move
    {
        // moves.emplace_back(std::nullopt);
    }

    std::vector<Move> out;
    out.reserve(N);

    // make sure not to sample the same move twice
    RNG rng_1;

    std::set<int> sampled_indices;

    for (int i = 0; i < N; i++) {
        int index = rng_1.getRand(moves.size());
        while (sampled_indices.find(index) != sampled_indices.end()) {
            index = rng_1.getRand(moves.size());
        }

        sampled_indices.insert(index);

        out.emplace_back(moves[index]);
    }

    return out;
}

Outcomes VersusGame::get_winner() const {
    Outcomes out = Outcomes::NONE;

    if (Shaktris::Utility::collides(p1_game.board, p1_game.current_piece)) {
        out = Outcomes::P2_WIN;
    }

    if (Shaktris::Utility::collides(p2_game.board, p2_game.current_piece)) {
        if (out == Outcomes::P2_WIN) {
            out = Outcomes::DRAW;
        } else {
            out = Outcomes::P1_WIN;
        }
    }

    return out;
}

void VersusGame::set_move(int id, Move move) {
    if (id == 0) {
        p1_move = move;
    } else {
        p2_move = move;
    }
}
