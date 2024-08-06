#include "Game.hpp"

#include <algorithm>
#include <bit>
#include <iostream>
#include <map>
#include <random>
#include <tuple>



/// <summary>
/// this function returns the size of the zero_g_starts
/// </summary>
/// <returns>size_t for the zero_g_starts of any given piece</returns>
consteval static size_t zero_g_starts_size(const PieceType p) {
    Piece initial_piece = Piece(p);

    auto collides = [](const Piece& piece) {

        for (auto& mino : piece.minos) {
            int x_pos = mino.x + piece.position.x;
            if (x_pos < 0 || x_pos >= Board::width)
                return true;

            int y_pos = mino.y + piece.position.y;
            if (y_pos < 0 || y_pos >= Board::height)
                return true;
        }

        return false;
        };

    auto shift = [&](Piece& piece, int dir) {
        piece.position.x += dir;

        if (collides(piece))
            piece.position.x -= dir;
        else
            piece.spin = spinType::null;
        };

    std::vector<Piece> open_nodes;
    open_nodes.reserve(150);
    std::vector<Piece> next_nodes;
    next_nodes.reserve(150);
    std::vector<bool> visited = std::vector<bool>(6444);

    std::vector<Piece> valid_moves;
    valid_moves.reserve(150);

    // rotations
    for (int r = 0; r < RotationDirections_N; r++) {
        open_nodes.emplace_back(initial_piece);
        if (initial_piece.type != PieceType::O)
            initial_piece.rotate(TurnDirection::Right);

        // lateral movement
        while (open_nodes.size() > 0) {
            // expand edges
            for (const auto& piece : open_nodes) {
                auto h = piece.compact_hash();
                if (visited[h])
                    continue;
                // mark node as visited
                visited[h] = true;

                valid_moves.push_back(piece);

                Piece new_piece = piece;
                shift(new_piece, -1);
                next_nodes.emplace_back(new_piece);

                new_piece = piece;
                shift(new_piece, 1);
                next_nodes.emplace_back(new_piece);
            }
            open_nodes = next_nodes;
            next_nodes.clear();
        }
    }

    return valid_moves.size();
};



// returns the zero g starts for the given piece
template<PieceType p>
consteval static auto zero_g_starts() {

    constexpr std::array<size_t, static_cast<size_t>(PieceType::PieceTypes_N)> zero_g_start_size = {
        zero_g_starts_size(PieceType::S),
        zero_g_starts_size(PieceType::Z),
        zero_g_starts_size(PieceType::J),
        zero_g_starts_size(PieceType::L),
        zero_g_starts_size(PieceType::T),
        zero_g_starts_size(PieceType::O),
        zero_g_starts_size(PieceType::I),
    };

    Piece initial_piece = Piece(p);

    auto collides = [](const Piece& piece) {

        for (auto& mino : piece.minos) {
            int x_pos = mino.x + piece.position.x;
            if (x_pos < 0 || x_pos >= Board::width)
                return true;

            int y_pos = mino.y + piece.position.y;
            if (y_pos < 0 || y_pos >= Board::height)
                return true;
        }

        return false;
        };

    auto shift = [&](Piece& piece, int dir) {
        piece.position.x += dir;

        if (collides(piece))
            piece.position.x -= dir;
        else
            piece.spin = spinType::null;
        };

    std::vector<Piece> open_nodes;
    open_nodes.reserve(150);
    std::vector<Piece> next_nodes;
    next_nodes.reserve(150);
    std::vector<bool> visited = std::vector<bool>(6444);

    std::vector<Piece> valid_moves;
    valid_moves.reserve(150);

    // rotations
    for (int r = 0; r < 4; r++) {
        open_nodes.emplace_back(initial_piece);
        if (initial_piece.type != PieceType::O)
            initial_piece.rotate(TurnDirection::Right);

        // lateral movement
        while (open_nodes.size() > 0) {
            // expand edges
            for (const auto& piece : open_nodes) {
                auto h = piece.compact_hash();
                if (visited[h])
                    continue;
                // mark node as visited
                visited[h] = true;

                valid_moves.push_back(piece);

                Piece new_piece = piece;
                shift(new_piece, -1);
                next_nodes.emplace_back(new_piece);

                new_piece = piece;
                shift(new_piece, 1);
                next_nodes.emplace_back(new_piece);
            }
            open_nodes = next_nodes;
            next_nodes.clear();
        }
    }

    // shoutouts to friedkeenan
    // github link: https://github.com/friedkeenan/
    auto zero_g_starts = [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
        return std::array{
            valid_moves[Indices]...
        };
    }(std::make_index_sequence<zero_g_start_size[(size_t)p]>{});

    return zero_g_starts;
}

constexpr auto zero_g_starts_S = zero_g_starts<PieceType::S>();
constexpr auto zero_g_starts_Z = zero_g_starts<PieceType::Z>();
constexpr auto zero_g_starts_J = zero_g_starts<PieceType::J>();
constexpr auto zero_g_starts_L = zero_g_starts<PieceType::L>();
constexpr auto zero_g_starts_T = zero_g_starts<PieceType::T>();
constexpr auto zero_g_starts_O = zero_g_starts<PieceType::O>();
constexpr auto zero_g_starts_I = zero_g_starts<PieceType::I>();






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

bool Game::collides(const Board& board, const Piece& piece) const {
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

void Game::rotate(Piece& piece, TurnDirection dir) const {
    const RotationDirection prev_rot = piece.rotation;

    piece.rotate(dir);

    const std::array<Coord, srs_kicks>* offsets = &piece_offsets_JLSTZ[piece.rotation];
    const std::array<Coord, srs_kicks>* prev_offsets = &piece_offsets_JLSTZ[prev_rot];

    if (piece.type == PieceType::I) {
        prev_offsets = &piece_offsets_I[prev_rot];
        offsets = &piece_offsets_I[piece.rotation];
    } else if (piece.type == PieceType::O) {
        prev_offsets = &piece_offsets_O[prev_rot];
        offsets = &piece_offsets_O[piece.rotation];
    }

    auto x = piece.position.x;
    auto y = piece.position.y;

    for (int i = 0; i < srs_kicks; i++) {
        piece.position.x = x + (*prev_offsets)[i].x - (*offsets)[i].x;
        piece.position.y = y + (*prev_offsets)[i].y - (*offsets)[i].y;
        if (!collides(board, piece)) {
            if (piece.type == PieceType::T) {
                constexpr std::array<std::array<Coord, 4>, 4> corners = {{
                    // a       b       c        d
                    {{{-1, 1}, {1, 1}, {1, -1}, {-1, -1}}},  // North
                    {{{1, 1}, {1, -1}, {-1, -1}, {-1, 1}}},  // East
                    {{{1, -1}, {-1, -1}, {-1, 1}, {1, 1}}},  // South
                    {{{-1, -1}, {-1, 1}, {1, 1}, {1, -1}}},  // West
                }};

                bool filled[4] = {true, true, true, true};

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
                    } else
                        piece.spin = spinType::mini;
                } else
                    piece.spin = spinType::null;
            }
            return;
        }
    }

    piece.position.x = x;
    piece.position.y = y;

    piece.rotate(dir == TurnDirection::Left ? TurnDirection::Right : TurnDirection::Left);
}

void Game::shift(Piece& piece, int dir) const {
    piece.position.x += dir;

    if (collides(board, piece))
        piece.position.x -= dir;
    else
        piece.spin = spinType::null;
}

void Game::sonic_drop(const Board board, Piece& piece) const {
    int distance = 32;
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
            shift(piece, -1);
            break;
        case Movement::Right:
            shift(piece, 1);
            break;
        case Movement::RotateClockwise:
            rotate(piece, TurnDirection::Right);
            break;
        case Movement::RotateCounterClockwise:
            rotate(piece, TurnDirection::Left);
            break;
        case Movement::SonicDrop:
            sonic_drop(board, piece);
            break;
            // default:
            // std::unreachable();
    }
}


// movegen for if the board is convex
std::vector<Piece> Game::movegen_fast(PieceType piece_type) const {
    Piece initial_piece = Piece(piece_type);

    std::vector<Piece> open_nodes;
    open_nodes.reserve(150);
    std::vector<Piece> next_nodes;
    next_nodes.reserve(150);
    std::vector<bool> visited = std::vector<bool>(6444);

    std::vector<Piece> valid_moves;
    valid_moves.reserve(150);

    // rotations
    for (int r = 0; r < 4; r++) {
        open_nodes.emplace_back(initial_piece);

        initial_piece.rotate(TurnDirection::Right);

        // lateral movement
        while (open_nodes.size() > 0) {
            // expand edges
            for (auto& piece : open_nodes) {
                auto h = piece.compact_hash();
                if (visited[h])
                    continue;
                // mark node as visited
                visited[h] = true;

                valid_moves.push_back(piece);

                Piece new_piece = piece;
                process_movement(new_piece, Movement::Left);
                next_nodes.emplace_back(new_piece);

                new_piece = piece;
                process_movement(new_piece, Movement::Right);
                next_nodes.emplace_back(new_piece);
            }
            open_nodes = next_nodes;
            next_nodes.clear();
        }
    }

    for (Piece& piece : valid_moves) {
        process_movement(piece, Movement::SonicDrop);
    }

    return valid_moves;
}

// Movegen for a convex board with free movement at the top. (decided by board height of ???)
std::vector<Piece> Game::movegen_faster(PieceType piece_type) const {

    std::vector<Piece> valid_moves;

    switch (piece_type) {
    case PieceType::S:
        valid_moves.assign(zero_g_starts_S.begin(), zero_g_starts_S.end());
        break;
    case PieceType::Z:
        valid_moves.assign(zero_g_starts_Z.begin(), zero_g_starts_Z.end());
        break;
    case PieceType::J:
        valid_moves.assign(zero_g_starts_J.begin(), zero_g_starts_J.end());
        break;
    case PieceType::L:
        valid_moves.assign(zero_g_starts_L.begin(), zero_g_starts_L.end());
        break;
    case PieceType::T:
        valid_moves.assign(zero_g_starts_T.begin(), zero_g_starts_T.end());
        break;
    case PieceType::O:
        valid_moves.assign(zero_g_starts_O.begin(), zero_g_starts_O.end());
        break;
    case PieceType::I:
        valid_moves.assign(zero_g_starts_I.begin(), zero_g_starts_I.end());
        break;
    case PieceType::Empty:
        break;
    case PieceType::PieceTypes_N:
        break;
    // default:
        //break;
    }

    for (Piece &piece : valid_moves) {
        process_movement(piece, Movement::SonicDrop);
    }

    return valid_moves;
}

bool Game::is_convex() const {
    Board shifted_board;

    int garbage_height = get_garbage_height();

    bool convex = true;

    for (int i = 0; i < Board::width; ++i) {
        shifted_board.board[i] = board.board[i] >> garbage_height;
    }

    for (int i = 0; i < Board::width; ++i) {
        auto& col = shifted_board.board[i];
        convex = convex && (std::popcount(col) == std::countr_one(col));
    }

    return convex;

}

int Game::get_garbage_height() const {

    int max_air = -1;

    for (int i = 0; i < Board::width; ++i) {
        auto& col = board.board[i];
        int air = std::countl_zero(col);
        max_air = std::max(air, max_air);
    }

    return 32 - max_air;
}

std::vector<Piece> Game::movegen(PieceType piece_type) const {
    {
        auto is_low = [](const Board& board) {
            constexpr auto p = Piece(PieceType::I);

            constexpr uint32_t high_collider = ~((1 << p.position.y) - 1);

            bool ret = true;

            for (size_t x = 0; x < Board::width; x++) {
                if (board.get_column(x) & high_collider)
                    ret = false;
            }
            return ret;
            };
        bool convex = is_convex();
        bool low = is_low(board);
        if(convex && low){
            return movegen_faster(piece_type);
        } else if (convex) {
            return movegen_fast(piece_type);
        }
    }
    Piece initial_piece = Piece(piece_type);

    std::vector<Piece> open_nodes;
    open_nodes.reserve(150);
    std::vector<Piece> next_nodes;
    next_nodes.reserve(150);
    std::vector<bool> visited = std::vector<bool>(6444);

    std::vector<Piece> valid_moves;
    valid_moves.reserve(100);

    // root node
    open_nodes.emplace_back(initial_piece);

    while (open_nodes.size() > 0) {
        // expand edges
        for (auto& piece : open_nodes) {
            auto h = piece.compact_hash();
            if (visited[h])
                continue;
            // mark node as visited
            visited[h] = true;

            // try all movements
            Piece new_piece = piece;
            process_movement(new_piece, Movement::RotateCounterClockwise);
            next_nodes.emplace_back(new_piece);

            new_piece = piece;
            process_movement(new_piece, Movement::RotateClockwise);
            next_nodes.emplace_back(new_piece);

            new_piece = piece;
            process_movement(new_piece, Movement::Left);
            next_nodes.emplace_back(new_piece);

            new_piece = piece;
            process_movement(new_piece, Movement::Right);
            next_nodes.emplace_back(new_piece);

            new_piece = piece;
            process_movement(new_piece, Movement::SonicDrop);
            next_nodes.emplace_back(new_piece);

            // check if the piece is grounded and therefore valid

            piece.position.y--;

            if (collides(board, piece)) {
                piece.position.y++;
                valid_moves.emplace_back(piece);
            }
        }
        open_nodes = next_nodes;
        next_nodes.clear();
    }

    return valid_moves;
}

// warning! if there is a piece in the hold and the current piece is empty, we dont use the hold
std::vector<Piece> Game::get_possible_piece_placements() const {
    // we exausted the queue
    if (current_piece.type == PieceType::Empty)
        return {};

    std::vector<Piece> valid_pieces = movegen(current_piece.type);

    PieceType holdType = hold.has_value() ? hold.value() : queue.front();
    if (holdType != PieceType::Empty) {
        std::vector<Piece> hold_pieces = movegen(holdType);
        valid_pieces.reserve(valid_pieces.size() + hold_pieces.size());
        for (auto& piece : hold_pieces) {
            valid_pieces.emplace_back(piece);
        }
    }

    return valid_pieces;
}