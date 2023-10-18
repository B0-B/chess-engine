#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <ctype.h>

using namespace std;

class snapshot {
    
    /* Constructor class for storing the game state ideally before a move. 
    This allows jumps between different move counts, similar to loading a fen. */

    public:

        int color = 0;
        int full_moves;

        map<int, vector<string>> pgn_history;
        string en_passant_coord = "-";

        bool white_is_checked = false;
        bool black_is_checked = false;

        // char capture_symbol = '_';
        bool all_castling_rights_lost = 0;
        bool castling_right_k_w = 1;
        bool castling_right_q_w = 1;
        bool castling_right_k_b = 1;
        bool castling_right_q_b = 1;
        string  white_king_coord = "E1", 
                black_king_coord = "E8";

        // occupation, moves and targets
        map<string, char> white_occupation_map,
                          black_occupation_map;
        map<string, vector<string>> white_targets,
                                    black_targets,
                                    white_moves,
                                    black_moves;
        map<string, vector<string>> white_pins,    // pin to spaces mapping for pinned pieces
                                    black_pins;
        map<string, vector<string>> white_spaces,  // spaces between attacker and king when in scoped check (no night nor pawn)
                                    black_spaces; 
        
        snapshot(void) {
            
        }
};