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

#include <immintrin.h>
int main() {

	Board board;

	Piece piece = PieceType::J;

	Shaktris::Utility::sonic_drop(board, piece);
	piece.position.y += 5;
	//board.set(piece);
	piece.position.x += 2;
	//board.set(piece);

	std::cout << std::bitset<32>(4294967295).count() << std::endl;
	return 0;
	std::vector<Piece> moves;
	constexpr auto count = 1;

	auto start = std::chrono::steady_clock::now();
	for (int i = 0; i < count; ++i) {
		moves = Shaktris::MoveGen::Smeared::god_movegen(board, piece.type);
	}
	auto end = std::chrono::steady_clock::now();
	// print the amount of functions calls we can do in one second
	auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
	double s = count / (time / 1e+9);
	std::cout << "time: " << time << std::endl;

	// print moves variable
	std::cout << "number of moves: " << moves.size() << std::endl;
	for (auto& move : moves) {
		// if you cant find the move in real_moves print
		std::cout << "type: " << (int)move.type << " x: " << (int)move.position.x << " y: " << (int)move.position.y << " rotation: " << rot_to_char(move.rotation) << std::endl;
	}
	auto real_moves = Shaktris::MoveGen::Traditional::movegen(srs_rotate,board, piece.type);
	
	
	
	
	
	// print the number of moves

	std::cout << "number of real moves: " << real_moves.size() << std::endl;
	for (auto& move : real_moves) {
		//std::cout << "type: " << (int)move.type << " x: " << (int)move.position.x << " y: " << (int)move.position.y << " rotation: " << rot_to_char(move.rotation) << std::endl;
	}

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