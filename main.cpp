/* 

    C++ chess engine implementation.
    This code is inteded for C++ compiler version >=11.

    Copyright 2022 B0-B
    Permission is hereby granted, free of charge, to any person obtaining a copy of 
    this software and associated documentation files (the "Software"), to deal in the 
    Software without restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, subject to the 
    following conditions:
    The above copyright notice and this permission notice shall be included in all copies 
    or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
    PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

    _________________________________________________________________________________________

*/

#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <map>
#include <string>
#include <ctype.h>
#include <cctype>
#include <vector>
#include <chrono>

// #include "general.h"
// #include "board.h"
#include "engine.h"

using namespace std;


int main (void) {

    // initialize a new board and pieces objects
    // Piece pieces;
    Board boardObject;
    boardObject.load_starting_position();
    boardObject.show_board();

    // test bug which breaks shannon simulation
    boardObject.show_active_color();
    MoveInfo move_1 = boardObject.active_move("A2", "A4");
    boardObject.show_board();
    MoveInfo move_2 = boardObject.active_move("B7", "B5");
    boardObject.show_board();
    MoveInfo move_3 = boardObject.active_move("A4", "B5");
    boardObject.show_board();
    // MoveInfo move_4 = boardObject.active_move("F7", "F6");
    // boardObject.show_board();
    // MoveInfo move_5 = boardObject.active_move("D1", "H5");
    // boardObject.show_board();
    // boardObject.show_moves_for_active_color();
    boardObject.show_pgn();
    boardObject.show_call_stats();
    // cout << "pgn finished." <<endl;
    // boardObject.show_moves_for_active_color();

    // cout << "test " << boardObject.show_moves_for_active_color() << endl;
    // undo all moves
//     boardObject.undo_active_move(move_5);
//     boardObject.show_board();
//     boardObject.undo_active_move(move_4);
//     boardObject.show_board();
    // boardObject.undo_active_move(move_3);
    // boardObject.show_board();
    // boardObject.undo_active_move(move_2);
    // boardObject.show_board();
    // boardObject.undo_active_move(move_1);
    // boardObject.show_board();
    // boardObject.show_pgn();
    // cout << "pgn finished." <<endl;

    // boardObject.show_active_color();
    // move_1 = boardObject.active_move("E2", "E4");
    // boardObject.show_board();
    // boardObject.show_pgn();
    
    // Engine engine;

    // shannon number test (number of positions for diven depth)
    // int depth = 5;
    // engine.shannon_number_simulation(depth, 0, 0);
    // engine.sequence_count_simulation(5, 0, 0);
    // engine.shannon_benchmark(depth);
    // engine.board_test.show_board();
    // boardObject.show_moves_for_active_color();
    // boardObject.show_move_count_for_active_color();

    // play the scandinavian for testing
    // boardObject.active_move("E2", "E4");
    // boardObject.active_move("D7", "D5");
    // boardObject.active_move("E4", "D5");
    
    // output
    // boardObject.show_pgn();

    // int id = 0;
    // cout << "id test " << id << " " << boardObject.get_coord_from_id(id);
    // boardObject.load_starting_position();
    
    // boardObject.show_half_clock();
    // boardObject.show_move_count();

    // boardObject.show_en_passant();
    // boardObject.show_castling_rights();

    // boardObject.refresh_position();


    // boardObject.show_move_count_for_active_color();
    // boardObject.show_moves_for_active_color();
    // boardObject.show_reachable_squares_for_active_color();
    
    // boardObject.show_position_activity();
    // boardObject.ignorant_move("E2", "E4");
    // boardObject.ignorant_move("D7", "D5");
    // boardObject.show_reachable_squares("E4");
    // boardObject.show_material();
    //boardObject.show_position_activity();
    
    return 0;
}