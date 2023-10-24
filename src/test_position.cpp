/* Load postion from fen and analyze bord moves. */

#include "engine.h"
#include <stdio.h>
#include <iostream>

using namespace std;

// position FEN string HERE:
// const string fen = "4k3/8/8/1b6/8/8/8/4K2R w K - 0 1";
const string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

int main (void) {

    Board _board;

    _board.load_position_from_fen(fen);
    _board.show_board();
    
    // count the moves found
    int possible_moves;
    for (auto const& y : _board.get_moves(_board.active_color))
        possible_moves += y.second.size();
    print("analyzed " + to_string(possible_moves) + 
        " possible moves.", "test");
    
    // show active moves
    _board.show_moves(_board.active_color);

}