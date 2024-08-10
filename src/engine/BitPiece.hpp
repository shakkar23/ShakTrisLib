#pragma once

#include <array>
#include <utility>

#include "ShaktrisConstants.hpp"

using piece_data_t = std::array<column_t, 4>;

constexpr auto generate_bit_piece_defs() {
	std::array<std::array<piece_data_t, (size_t)RotationDirections_N>, (size_t)PieceType::PieceTypes_N> bit_piece_rot_def{};

	for (int type = 0; type < (int)PieceType::PieceTypes_N; type++) {

		if (type != (int)PieceType::I && type != (int)PieceType::O) {
			// found the type in trivial pieces

			Piece piece = Piece((PieceType)type);

			for (int rot = 0; rot < (int)RotationDirections_N; ++rot) {

				for (const auto& coord : piece.minos) {
					bit_piece_rot_def[type][rot][coord.x + 1] |= 1 << (coord.y + 1);
				}

				piece.rotate(TurnDirection::Right);
			}
		}
	}

	// do the O piece

	for (int rot = 0; rot < (int)RotationDirections_N; ++rot) {
		bit_piece_rot_def[(size_t)PieceType::O][rot][0] = 0b0000;
		bit_piece_rot_def[(size_t)PieceType::O][rot][1] = 0b0110;
		bit_piece_rot_def[(size_t)PieceType::O][rot][2] = 0b0110;
		bit_piece_rot_def[(size_t)PieceType::O][rot][3] = 0b0000;
	}

	// hardcoded I piece because writing a rotation function for it is a pain
	bit_piece_rot_def[(size_t)PieceType::I][0][0] = 0b0100;
	bit_piece_rot_def[(size_t)PieceType::I][0][1] = 0b0100;
	bit_piece_rot_def[(size_t)PieceType::I][0][2] = 0b0100;
	bit_piece_rot_def[(size_t)PieceType::I][0][3] = 0b0100;

	bit_piece_rot_def[(size_t)PieceType::I][1][0] = 0b0000;
	bit_piece_rot_def[(size_t)PieceType::I][1][1] = 0b1111;
	bit_piece_rot_def[(size_t)PieceType::I][1][2] = 0b0000;
	bit_piece_rot_def[(size_t)PieceType::I][1][3] = 0b0000;

	bit_piece_rot_def[(size_t)PieceType::I][2][0] = 0b0010;
	bit_piece_rot_def[(size_t)PieceType::I][2][1] = 0b0010;
	bit_piece_rot_def[(size_t)PieceType::I][2][2] = 0b0010;
	bit_piece_rot_def[(size_t)PieceType::I][2][3] = 0b0010;

	bit_piece_rot_def[(size_t)PieceType::I][3][0] = 0b0000;
	bit_piece_rot_def[(size_t)PieceType::I][3][1] = 0b0000;
	bit_piece_rot_def[(size_t)PieceType::I][3][2] = 0b1111;
	bit_piece_rot_def[(size_t)PieceType::I][3][3] = 0b0000;


	return bit_piece_rot_def;
}

// bit_piece_defs[type][rot] to the get the piece
constexpr inline std::array<std::array<piece_data_t, (size_t)RotationDirections_N>, (size_t)PieceType::PieceTypes_N> bit_piece_rot_def = generate_bit_piece_defs();

class BitPiece final {
public:

	constexpr BitPiece(const PieceType& type) {

		rotation = RotationDirection::North;
		pos.y = 18;
		pos.x = 3;
		spin = spinType::null;
		this->type = type;

		bit_piece = bit_piece_rot_def[(size_t)type][(size_t)RotationDirection::North];

		for (auto& column : bit_piece) {
			column <<= pos.y;
		}
	};

	constexpr BitPiece(PieceType type, RotationDirection dir) noexcept {
		this->type = type;
		rotation = dir;
		pos = { 3, 18 };
		bit_piece = bit_piece_rot_def[static_cast<size_t>(type)][static_cast<size_t>(dir)];

		for (auto& column : bit_piece) {
			column <<= pos.y;
		}

		spin = spinType::null;
	}

	constexpr BitPiece(const BitPiece& other) {
		bit_piece = other.bit_piece;
		pos.x = other.pos.x;
		pos.y = other.pos.y;
		spin = other.spin;
		type = other.type;
	}

	constexpr inline void rotate(TurnDirection direction) {
		switch (direction)
		{
		case Left:
			rotate_left();
			break;
		case Right:
			rotate_right();
			break;
			//default:
				//break;
		}
	}

	constexpr inline uint32_t hash() const {
		return ((int)type << 24) | (pos.x << 16) | (pos.y << 8) | (rotation);
	}

	constexpr inline uint32_t compact_hash() const {
		return uint32_t(rotation) + uint32_t(pos.x) * n_minos + uint32_t(pos.y) * 10 * n_minos + (uint32_t)type * 10 * 20 * n_minos;
	}

	constexpr inline void rotate_right() {
		rotation = static_cast<RotationDirection>((static_cast<int>(rotation) + 1) % RotationDirections_N);

		// bring down the piece to the bottom to do the matrix rotation
		for (auto& column : bit_piece) {
			column >>= pos.y;
		}

		// perform 4x4 matrix rotation
		piece_data_t new_piece{};
		for (size_t i = 0; i < 4; i++) {
			for (size_t j = 0; j < 4; j++) {
				new_piece[i] |= ((bit_piece[j] >> i) & 1) << (3 - j);
			}
		}

		// bring the piece back up
		for (auto& column : new_piece) {
			column <<= pos.y;
		}

		bit_piece = new_piece;
	}

	constexpr inline void rotate_left() {
		rotation = static_cast<RotationDirection>((static_cast<int>(rotation) + (RotationDirections_N - 1)) % RotationDirections_N);

		// bring down the piece to the bottom to do the matrix rotation
		for (auto& column : bit_piece) {
			column >>= pos.y;
		}

		// perform 4x4 counter clockwise matrix rotation
		piece_data_t new_piece{};
		for (size_t i = 0; i < 4; i++) {
			for (size_t j = 0; j < 4; j++) {
				new_piece[i] |= ((bit_piece[3 - j] >> i) & 1) << j;
			}
		}


		// bring the piece back up
		for (auto& column : new_piece) {
			column <<= pos.y;
		}

		bit_piece = new_piece;

	}

	piece_data_t bit_piece;
	// the position is the bottom left corner of the piece
	Coord pos;
	RotationDirection rotation;
	PieceType type;
	spinType spin;
};