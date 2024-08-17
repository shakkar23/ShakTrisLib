#pragma once

#include <array>
#include <cstdint>

typedef uint8_t u8;    ///<   8-bit unsigned integer.
typedef int8_t i8;     ///<   8-bit signed integer.
typedef uint16_t u16;  ///<  16-bit unsigned integer.
typedef uint32_t u32;  ///<  32-bit unsigned integer.
typedef uint64_t u64;  ///<  64-bit unsigned integer.


using column_t = u32;

enum class spinType : u8 {
    null,
    mini,
    normal,
};
struct Coord {
    i8 x;
    i8 y;
};

enum RotationDirection : u8 {
    North,
    East,
    South,
    West,
    RotationDirections_N
};

enum ColorType : u8 {

    // Color for pieces
    S,
    Z,
    J,
    L,
    T,
    O,
    I,
    // special types
    Empty,
    LineClear,
    Garbage,
    ColorTypes_N
};

enum class PieceType : u8 {
    // actual pieces
    S = ColorType::S,
    Z = ColorType::Z,
    J = ColorType::J,
    L = ColorType::L,
    T = ColorType::T,
    O = ColorType::O,
    I = ColorType::I,
    Empty = ColorType::Empty,
    PieceTypes_N
};

enum TurnDirection : u8 {
    Left,
    Right,
};

enum class Movement : u8 {
    Left,
    Right,
    RotateClockwise,
    RotateCounterClockwise,
    SonicDrop,
};

// number of kicks srs has, including for initial
constexpr std::size_t n_minos = 4;

constexpr std::array<std::array<Coord, n_minos>, (int)PieceType::PieceTypes_N> piece_definitions = {

    {
        {{{-1, 0}, {0, 0}, {0, 1}, {1, 1}}},   // S
        {{{-1, 1}, {0, 1}, {0, 0}, {1, 0}}},   // Z
        {{{-1, 0}, {0, 0}, {1, 0}, {-1, 1}}},  // J
        {{{-1, 0}, {0, 0}, {1, 0}, {1, 1}}},   // L
        {{{-1, 0}, {0, 0}, {1, 0}, {0, 1}}},   // T
        {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}},    // O
        {{{-1, 0}, {0, 0}, {1, 0}, {2, 0}}},   // I
        {{{0, 0}, {0, 0}, {0, 0}, {0, 0}}}     // NULL
    }};
/// from this point onwards we define things with constexpr functions

/// <summary>
/// generates the rotation piece definition for the piece to just have a lookup for every rotation state
/// </summary>
/// <returns>the rot_piece_def</returns>
consteval auto generate_rot_piece_def() {
    std::array<std::array<std::array<Coord, n_minos>, (int)PieceType::PieceTypes_N>, RotationDirection::RotationDirections_N> rot_piece_def{};

    for (size_t type = 0; type < piece_definitions.size(); ++type) {
        auto minos = piece_definitions[type];
        // this relies on the ordering of the directions to be: N E S W
        for (size_t rot = 0; rot < RotationDirection::RotationDirections_N; ++rot) {
            // set the minos
            for (size_t mino = 0; mino < n_minos; ++mino) {
                rot_piece_def[rot][type][mino] = minos[mino];
            }

            // rotate them to the right
            for (auto& mino : minos) {
                Coord temp_mino = mino;
                temp_mino.x *= -1;
                mino = { temp_mino.y, temp_mino.x };
            }

        }
    }

    return rot_piece_def;
}

// rot_piece_def[rot][type]
constexpr std::array<std::array<std::array<Coord, n_minos>, (int)PieceType::PieceTypes_N>, RotationDirection::RotationDirections_N> rot_piece_def = generate_rot_piece_def();

constexpr i8 piece_spawn_height = 19;
