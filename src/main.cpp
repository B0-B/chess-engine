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
// #include "board_new.h"

using namespace std;


int main (void) {

    Board _board;

    // cout << "test unicode: " << _board.pieces.to_unicode('B'); 

    _board.load_starting_position();
    // _board.show_occupation(_board.pieces.w);
    // _board.show_targets(_board.pieces.w);
    // _board.show_moves(_board.pieces.w);
    _board.show_board();
    _board.move("E2", "E4");
    _board.show_board();
    _board.move("E7", "E5");
    _board.show_board();
    
    // Engine ce;
    // ce.board_test.show_board();
    // ce.shannon_number_simulation(4, 0, 1);
    
    // vector<int> vec;
    // vec.reserve(100);
    // for (size_t i = 0; i < 10; i++)
    // {
    //     /* code */
    //     vec.push_back(i);
    // }
    // cout << vec[10] << endl;
    // cout << vec.size() << endl;
    

    return 0;
}