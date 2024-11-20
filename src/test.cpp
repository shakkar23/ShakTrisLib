#include "engine/Board.hpp"
#include "engine/MoveGen.hpp"


#include <iostream>
#include <chrono>
#include <numeric>

char rot_to_char(RotationDirection rot) {
	switch (rot) {
	case RotationDirection::North:
		return 'N';
	case RotationDirection::East:
		return 'E';
	case RotationDirection::South:
		return 'S';
	case RotationDirection::West:
		return 'W';
	}
	return 'X';
}


static uint64_t perft(Board board, Piece move, const std::vector<Piece> &queue, int i, int depth) {
    board.set(move);

    if (depth <= 1) {
        return Shaktris::MoveGen::Smeared::god_movegen(board, queue[i].type).size();
    }

    uint64_t nodes = 0;

    for (auto& move : Shaktris::MoveGen::Smeared::god_movegen(board, queue[i].type)) {
        nodes += perft(board, move, queue, i + 1, depth - 1);
    }
    return nodes;
}

static uint64_t perft(Board board, const std::vector<Piece>& queue, int depth) {
    if (depth <= 1) {
        return Shaktris::MoveGen::Smeared::god_movegen(board, queue[0].type).size();
    }

    uint64_t nodes = 0;

    for (auto& move : Shaktris::MoveGen::Smeared::god_movegen(board, queue[0].type)) {
        nodes += perft(board, move, queue, 1, depth - 1);
    }
    return nodes;
}
int main() {
    std::vector<Piece> queue {
        // IOTLJSZ
        Piece(PieceType::I),
        Piece(PieceType::O),
        Piece(PieceType::T),
        Piece(PieceType::L),
        Piece(PieceType::J),
        Piece(PieceType::S),
        Piece(PieceType::Z)
    };

    auto now = std::chrono::high_resolution_clock::now();
    uint64_t nodes = perft(Board(), queue, 7);
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "number of nodes: " << nodes << std::endl;
    std::cout << "time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - now).count() << "ms" << std::endl;
    std::cout << "nodes per second: " << nodes / (std::chrono::duration_cast<std::chrono::milliseconds>(end - now).count() / 1000.0) << std::endl;

    // Board board;

    // Piece piece = PieceType::J;

    // Shaktris::Utility::sonic_drop(board, piece);
    // board.set(piece);
    // piece.position.y += 5;
    // piece.position.x += 2;
    // //board.set(piece);

    // //std::cout << std::bitset<32>(4294967295).count() << std::endl;
    // //return 0;
    // std::vector<Piece> moves;
    // constexpr auto count = 25'000'000;

    // auto start = std::chrono::steady_clock::now();
    // for (int i = 0; i < count; ++i) {
    // 	moves = Shaktris::MoveGen::Smeared::god_movegen(board, piece.type);
    // }
    // auto end = std::chrono::steady_clock::now();
    // // print the amount of functions calls we can do in one second
    // auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    // double s = count / (time / 1e+9);
    // std::cout << "time: " << s << std::endl;

    // // print moves variable
    // std::cout << "number of moves: " << moves.size() << std::endl;
    // for (auto& move : moves) {
    // 	// if you cant find the move in real_moves print
    // 	std::cout << "type: " << (int)move.type << " x: " << (int)move.position.x << " y: " << (int)move.position.y << " rotation: " << rot_to_char(move.rotation) << std::endl;
    // }
    // auto real_moves = Shaktris::MoveGen::Traditional::movegen(srs_rotate,board, piece.type);

    // // print the number of moves

    // std::cout << "number of real moves: " << real_moves.size() << std::endl;
    // for (auto& move : real_moves) {
    // 	//std::cout << "type: " << (int)move.type << " x: " << (int)move.position.x << " y: " << (int)move.position.y << " rotation: " << rot_to_char(move.rotation) << std::endl;
    // }

    /*
    // print the smeared board
    for(int b = 0; b < 4; b++) {
            std::cout << std::array{ "north", "east", "south", "west" } [b] << std::endl;
            for (int y = Board::visual_height - 1; y >= 0; y--) {
                    for (size_t x = 0; x < Board::width; x++) {
                            std::cout << smeared.boards[b].get(x, y);
                    }
                    std::cout << std::endl;
            }

            std::cout << std::endl;
            std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << std::endl;

    // print the normal board
    for (int y = Board::visual_height - 1; y >= 0; y--) {
            for (size_t x = 0; x < Board::width; x++) {
                    std::cout << board.get(x, y);
            }
            std::cout << std::endl;
    }
    auto def = rot_piece_def[static_cast<size_t>(2)][static_cast<size_t>(PieceType::T)];
    // print the piece def

    for (int i = 0; i < 4; i++) {
            std::cout << (int)def[i].x << " " << (int)def[i].y << std::endl;
            std::cout << std::endl;
    } */

    return 0;
}