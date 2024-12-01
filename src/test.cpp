#include <chrono>
#include <iomanip>  // for std::setw and std::setfill
#include <iostream>
#include <numeric>
#include <cmath>

#include "engine/Board.hpp"
#include "engine/MoveGen.hpp"

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
        default:
            break;
    }
    return 'X';
}

char to_char(PieceType p) {
    switch (p) {
        case PieceType::I:
            return 'I';
        case PieceType::O:
            return 'O';
        case PieceType::T:
            return 'T';
        case PieceType::L:
            return 'L';
        case PieceType::J:
            return 'J';
        case PieceType::S:
            return 'S';
        case PieceType::Z:
            return 'Z';
        default:
            return 'X';
    }
};

void print_board(const Board& board) {
    for (int y = Board::visual_height - 1; y >= 0; y--) {
        for (size_t x = 0; x < Board::width; x++) {
            std::cout << board.get(x, y);
        }
        std::cout << std::endl;
    }
};

void Citrus() {
    std::array<PieceType, 7> queue{
        // IOTLJSZ
        PieceType::I,
        PieceType::O,
        PieceType::T,
        PieceType::L,
        PieceType::J,
        PieceType::S,
        PieceType::Z};

    using namespace std;

    Board board;

    auto func = [&](Board b) {
        constexpr int count = 1'000'000;

        // For each piece
        for (i8 t = 0; t < 7; ++t) {
            int64_t time = 0;
            int64_t c = 0;

            std::vector<int64_t> lists;
            lists.reserve(count);
            auto m = Shaktris::MoveGen::Smeared::god_movegen(b, queue[t]);

            for (int i = 0; i < count; ++i) {
                auto time_start = chrono::high_resolution_clock::now();
                auto m = Shaktris::MoveGen::Smeared::convex_movegen(b, queue[t]);
                auto time_stop = chrono::high_resolution_clock::now();

                auto dt = chrono::duration_cast<chrono::nanoseconds>(time_stop - time_start).count();

                c += m.size();
                time += dt;
                lists.push_back(dt);
            }

            // Calculate mean time & movegen count
            time = time / count;
            c = c / count;

            // Calculate stdev
            uint64_t max = std::max_element(lists.begin(), lists.end())[0];
            uint64_t min = std::min_element(lists.begin(), lists.end())[0];
            uint64_t sd = 0;

            for (auto dt : lists) {
                sd += (dt - time) * (dt - time);
            }

            sd = sd / count;

            cout << "\tpiece: " << to_char(queue[t]) << "\ttime: " << time << " ns" << "\tstdev: " << std::sqrt(sd) << "\tmin: " << min << " ns" << "\tmax: " << max << " ns" << "\tcount: " << c << endl;
        }
    };

    board.board[9] = 0b00111111;
    board.board[8] = 0b00111111;
    board.board[7] = 0b00011111;
    board.board[6] = 0b00000111;
    board.board[5] = 0b00000001;
    board.board[4] = 0b00000000;
    board.board[3] = 0b00001101;
    board.board[2] = 0b00011111;
    board.board[1] = 0b00111111;
    board.board[0] = 0b11111111;

    cout << "BOARD TSPIN" << endl;
    // auto pieces = Shaktris::MoveGen::Smeared::god_movegen(board, PieceType::J);
    // std::cout << "number of moves: " << pieces.size() << std::endl;
    // for(auto p:pieces){
    //     // print them all in a pretty way
    //     cout << " x: " << (int)p.position.x << " y: " << (int)p.position.y << " rotation: " << rot_to_char(p.rotation) << endl;
    //     /*cout << "\t";
    //     // print the minos in each piece sorted from top to bottom left to right
    //     std::sort(p.minos.begin(), p.minos.end(), [](Coord a, Coord b) {
    //         return a.y == b.y ? a.x < b.x : a.y < b.y;
    //     });

    //     for(auto m:p.minos){
    //         cout << "(" << (int)(p.position.x+m.x) << ", " << (int)(p.position.y+m.y) << ") ";
    //     }
    //     cout << endl
    //          << endl;*/
    // }
    // return;
    func(board);

    board.board[9] = 0b111111111;
    board.board[8] = 0b111111111;
    board.board[7] = 0b011111111;
    board.board[6] = 0b011111111;
    board.board[5] = 0b000111111;
    board.board[4] = 0b000100110;
    board.board[3] = 0b010000001;
    board.board[2] = 0b011110111;
    board.board[1] = 0b011111111;
    board.board[0] = 0b011111111;

    cout << "BOARD DT CANNON" << endl;
    func(board);

    board.board[9] = 0b000011111111;
    board.board[8] = 0b000011000000;
    board.board[7] = 0b110011001100;
    board.board[6] = 0b110011001100;
    board.board[5] = 0b110011001100;
    board.board[4] = 0b110011001100;
    board.board[3] = 0b110011001100;
    board.board[2] = 0b110000001100;
    board.board[1] = 0b110000001100;
    board.board[0] = 0b111111111100;

    cout << "BOARD TERRIBLE" << endl;
    func(board);

    return;
}
void Shakkar() {
    std::array<PieceType, 7> queue{
        // IOTLJSZ
        PieceType::I,
        PieceType::O,
        PieceType::T,
        PieceType::L,
        PieceType::J,
        PieceType::S,
        PieceType::Z};
    auto func = [&](Board b) {
        constexpr int count = 1'000'000;

        // For each piece
        for (i8 t = 0; t < 7; ++t) {
            for (int i = 0; i < count; ++i) {
                auto m = Shaktris::MoveGen::Smeared::god_movegen(b, queue[t]);
                
            }
        }
    };
    Board board;
    board.board[9] = 0b000011111111;
    board.board[8] = 0b000011000000;
    board.board[7] = 0b110011001100;
    board.board[6] = 0b110011001100;
    board.board[5] = 0b110011001100;
    board.board[4] = 0b110011001100;
    board.board[3] = 0b110011001100;
    board.board[2] = 0b110000001100;
    board.board[1] = 0b110000001100;
    board.board[0] = 0b111111111100;

    func(board);
}

void kise(){
    constexpr int depth = 6;

    std::array<PieceType, 7> queue{
        // IOTLJSZ
        PieceType::I,
        PieceType::O,
        PieceType::T,
        PieceType::L,
        PieceType::J,
        PieceType::S,
        PieceType::Z};

    auto now = std::chrono::steady_clock::now();
    Board board;

    auto func = [&](Board b) {
        const int count = 1000000;

        // For each piece
        for (i8 t = 0; t < 7; ++t) {
            int64_t time = 0;
            int64_t c = 0;

            std::vector<int64_t> lists;
            lists.reserve(1000000);

    // Print the formatted duration
    std::cout << std::setw(1) << minutes.count() << "m "
              << std::setw(1) << seconds.count() << "s "
              << std::setw(1) << milliseconds.count() << "ms" << std::endl;
}

int main() {
    Citrus();
    return 0;
        
        /*
            Board board;

            Piece piece = PieceType::J;

            Shaktris::Utility::sonic_drop(board, piece);
            board.set(piece);
            piece.position.y += 5;
            piece.position.x += 2;
            // board.set(piece);

            // std::cout << std::bitset<32>(4294967295).count() << std::endl;
            // return 0;
            std::vector<Piece> moves;
            constexpr auto count = 20'000'000;

            auto start = std::chrono::steady_clock::now();
            for (int i = 0; i < count; ++i) {
                moves = Shaktris::MoveGen::Smeared::god_movegen(board, piece.type);
            }
            auto end = std::chrono::steady_clock::now();
            // print the amount of functions calls we can do in one second
            auto duration = end - start;

            // Extract the components
            auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
            duration -= minutes;
            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
            duration -= seconds;
            auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);

            std::cout << std::setw(1) << minutes.count() << "m "
                      << std::setw(1) << seconds.count() << "s "
                      << std::setw(1) << milliseconds.count() << "ms" << std::endl;

            // print moves variable
            std::cout
                << "number of moves: " << moves.size() << std::endl;
            for (auto& move : moves) {
                // if you cant find the move in real_moves print
                std::cout << "type: " << (int)move.type << " x: " << (int)move.position.x << " y: " << (int)move.position.y << " rotation: " << rot_to_char(move.rotation) << std::endl;
            }
            auto real_moves = Shaktris::MoveGen::Traditional::movegen(srs_rotate, board, piece.type);

            // print the number of moves

            std::cout << "number of real moves: " << real_moves.size() << std::endl;
            for (auto& move : real_moves) {
                // std::cout << "type: " << (int)move.type << " x: " << (int)move.position.x << " y: " << (int)move.position.y << " rotation: " << rot_to_char(move.rotation) << std::endl;
            }

        */

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
