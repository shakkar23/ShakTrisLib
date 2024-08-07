#pragma once
#include <array>
#include <cmath>
#include <optional>
#include <ranges>
#include <vector>
#include <variant>

#include "Board.hpp"
#include "Move.hpp"
#include "Piece.hpp"
//#include "BitPiece.hpp"

#include "ShaktrisConstants.hpp"

#include "modes/TetrioS1.hpp"
#include "modes/Botris.hpp"


constexpr int QUEUE_SIZE = 6;

class Game {
   public:
    Game() : current_piece(PieceType::Empty) {
        for (auto& p : queue) {
            p = PieceType::Empty;
        }
        mode = Botris();
    }
    Game& operator=(const Game& other) {
        if (this != &other) {
            board = other.board;
            current_piece = other.current_piece;
            hold = other.hold;
            queue = other.queue;
            garbage_meter = other.garbage_meter;
            b2b = other.b2b;
            combo = other.combo;
            mode = other.mode;
        }
        return *this;
    }
    ~Game() {}

    void place_piece();

    bool place_piece(const Piece& piece);

    void do_hold();

    void add_garbage(int lines, int location);

    int damage_sent(int linesCleared, spinType spinType, bool pc);

    void process_movement(Piece& piece, Movement movement) const;

    std::vector<Piece> get_possible_piece_placements() const;

    Board board;
    Piece current_piece;
    std::optional<PieceType> hold;
    u8 garbage_meter = 0;
    u16 b2b = 0;
    u16 combo = 0;
    std::array<PieceType, QUEUE_SIZE> queue;

    // make a variant thats points callable
    std::variant<TetrioS1, Botris> mode;

};