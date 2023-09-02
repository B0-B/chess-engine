#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <ctype.h>

using namespace std;

class MoveInfo {
    
    /* Constructor class for storing move information. */

    public:

        bool legal = 0;
        int color = 0;
        int move_count;
        char symbol = '_';

        string en_passant_coord = "-";
        string origin = "";
        string target = "";
        string capture_coord = "";

        char capture_symbol = '_';
        bool castling_right_k_w = 1;
        bool castling_right_q_w = 1;
        bool castling_right_k_b = 1;
        bool castling_right_q_b = 1;
        
        map<string, vector<string>> moves;
        map<string, vector<string>> targets;
        
};