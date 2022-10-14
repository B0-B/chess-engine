/* 

C++ chess engine implementation.
This code is inteded for C++ compiler version >=11.
B0-B

*/



#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include <ctype.h>
#include <vector>

using namespace std;

/* Chess pieces implementation */
class Piece {

    /* A set of pieces constructor, allows to spawn 
    and differentiate every piece on the board */

    public:

        // denote raw nonce id for each peace
        const int None = 0;
        const int Pawn = 1;
        const int Knight = 2;
        const int Bishop = 3;
        const int Rook = 4;
        const int Queen = 5;
        const int King = 6;

        // seperate white and black
        const int w = 8;
        const int b = 16;

        // map read from top to bottom (decending rank from white's perspective)
        const float positional_weight_map[6][8][8] = {
            // pawn
            {
                {1., 1., 1., 1., 1., 1., 1., 1.},
                {.9, .9, .9, .9, .9, .9, .9, .9},
                {.8, .8, .8, .8, .8, .8, .8, .8},
                {.7, .7, .7, .7, .7, .7, .7, .7},
                {.5, .5, .5, .5, .5, .5, .5, .5},
                {.3, .3, .3, .3, .3, .3, .3, .3},
                {.1, .1, .1, .1, .1, .1, .1, .1},
                {0., 0., 0., 0., 0., 0., 0., 0.}
            },
            // knight
            {
                {.1, .2, .3, .4, .4, .3, .2, .1},
                {.3, .5, .7, .5, .5, .8, .5, .3},
                {.35, .4, .5, .7, .6, .5, .4, .35},
                {.25, .35, .4, .5, .5, .4, .35, .25},
                {.2, .3, .35, .4, .4, .35, .3, .2},
                {.1, .2, .3, .1, .1, .3, .2, .1},
                {0., 0., 0., .2, .2, 0., 0., 0.},
                {0., 0., 0., 0., 0., 0., 0., 0.}
            },
            // bishop
            {
                {.2, 0., 0., 0., 0., 0., 0., .2},
                {.2, .3, .4, .4, .4, .4, .3, .2},
                {.2, .25, .4, .6, .6, .4, .25, .2},
                {.15, .2, .4, .9, .9, .4, .2, .15},
                {.2, .25, .6, .85, .85, .6, .25, .2},
                {.2, .3, .5, .2, .2, .5, .3, .2},
                {.2, .4, .2, .1, .1, .2, .4, .2},
                {.3, 0., 0., 0., 0., 0., 0., .3}
            },
            // rook
            {
                {.2, 0., 0., 0., 0., 0., 0., .2},
                {.5, .6, .7, .8, .9, .8, .7, .5},
                {.2, .25, .4, .5, .8, .4, .25, .2},
                {.35, .6, .7, .6, .7, .75, .65, .4},
                {.3, .4, .6, .9, 1., .6, .45, .35},
                {.3, .3, .5, .2, .2, .5, .3, .2},
                {.2, .2, .2, .1, .1, .2, .4, .2},
                {.1, .1, 0., .2, .3, .2, 0., .1}
            },
            // queen
            {
                {.2, 0., 0., 0., 0., 0., 0., .2},
                {.2, .3, .4, .4, .4, .4, .3, .2},
                {.2, .25, .4, .6, .6, .4, .25, .2},
                {.15, .2, .4, .9, .9, .4, .2, .15},
                {.2, .25, .6, .85, .85, .6, .25, .2},
                {.2, .3, .5, .2, .2, .5, .3, .2},
                {.2, .4, .2, .1, .1, .2, .4, .2},
                {.3, 0., 0., 0., 0., 0., 0., .3}
            },
            // king
            {
                {0., 0., 0., 0., 0., 0., 0., 0.},
                {0., 0., 0., 0., 0., 0., 0., 0.},
                {0., 0., .1, .2, .2, .1, 0., 0.},
                {0., 0., .2, .3, .3, .2, 0., 0.},
                {0., 0., .2, .3, .3, .2, 0., 0.},
                {0., .1, .1, .2, .2, .1, .1, 0.},
                {0., .3, 0., .1, .1, 0., .4, 0.},
                {.6, .8, .4, .0, 0., .5, .9, .7}
            }
        };

        const string symbols = "_PNBRQKpnbrqk";
        

        int is_white (char symbol) {
            return symbol >= 'A' && symbol <= 'Z' && symbol != '_';
        };

        int from_symbol (char symbol) {

            /* Returns the corresponding piece id from FEN symbol */
            
            int i;
            for (i = 0; i < symbols.size(); i++) {
                if (symbols[i] == toupper(symbol)) {
                    break;
                }
            }

            return i;

        };

        string name_from_symbol (char symbol) {

            /* Returns color and piece name from FEN symbol. */

            const char sym = tolower(symbol);
            string name = "";

            if (is_white(symbol)) {
                name += "white ";
            } else {
                name += "black ";
            }

            // pick name based on symbol
            if (sym == 'p') {
                name += "pawn";
            } else if (sym == 'n') {
                name += "knight";
            } else if (sym == 'b') {
                name += "bishop";
            } else if (sym == 'r') {
                name += "rook";
            } else if (sym == 'q') {
                name += "queen";
            } else if (sym == 'k') {
                name += "king";
            }

            return name;
        }

        char to_symbol (int piece, int color) {

            /* Every nonce id and color id will generate a FEN-compliant symbol, 
            lower case chars for black and upper case for white, 
            _ indicates empty or None.*/

            if (color == 16) {
                return symbols[piece + 6];
            } else {
                return symbols[piece];
            }

        }

        wstring to_unicode (char symbol) {

            /* Returns unicode for each piece (or symbol) which 
            when printed shows renders a chess piece in console. */

            int piece = from_symbol(symbol);
            wstring unicode;

            //wstring s = L"\u2659";
            //wcout << "test unicode " << s.c_str() << endl;

 
            if (piece == 0) {
                unicode = '_';
            } else if (piece == 1) {
                if (is_white(piece)) {
                    unicode = L"\u2659";
                } else {
                    unicode = L"\u265F";
                }
            } else if (piece == 2) {
                if (is_white(piece)) {
                    unicode = L"\u2658";
                } else {
                    unicode = L"\u065E";
                }
            } else if (piece == 3) {
                if (is_white(piece)) {
                    unicode = L"\u0657";
                } else {
                    unicode = L"\u265D";
                }
            } else if (piece == 4) {
                if (is_white(piece)) {
                    unicode = L"\u2656";
                } else {
                    unicode = L"\u265C";
                }
            } else if (piece == 5) {
                if (is_white(piece)) {
                    unicode = L"\u2655";
                } else {
                    unicode = L"\u265B";
                }
            } else if (piece == 6) {
                if (is_white(piece)) {
                    unicode = L"\u2654";
                } else {
                    unicode = L"\u265A";
                }
            }
            return unicode;
        }

        int value (int piece) {

            /* Returns the historically known value of a piece. 
            The value is normalized by a pawn value i.e. 1. 
            https://www.chess.com/terms/chess-piece-value*/

            if (piece == 1)
                return 1;
            else if (piece == 2)
                return 3;
            else if (piece == 3)
                return 3;
            else if (piece == 4)
                return 5;
            else if (piece == 5)
                return 9;
            return 0;
        };

        int value_from_symbol (char symbol) {

            /* Returns the historically known value of a piece. 
            The value is normalized by a pawn value i.e. 1. */
            
            return value(from_symbol(symbol));
            
        }
};

/*  Chess Board Implementation */
class Board {   

    public:

        /* ---- General Parameters ---- */
        // Verbose mode which prints every operation
        bool __verbose__ = true; 


        // constructor sequence
        Board(void) {

            // initialize grid
            build_grid();
            
        };


        /* define get methods */
        int get_color_from_symbol (char symbol) {
            if (pieces.is_white(symbol))
                return 8;
            else if (symbol == '_')
                return 0;
            else
                return 16; 
        };

        string get_coord_from_id (int id) {
            return grid[id]["coordinate"];
        };

        string get_coord_from_file_and_rank (int file, int rank) {
            return get_coord_from_id(file + 8 * rank);
        };

        int get_id_from_coord (string coord_str) {

            /* Returns the id assigned to the square at req. coordinate */
            
            return stoi(get_square_from_coord(coord_str)["id"]);

        };

        map<string, string> get_square_from_coord (string coord_str) {

            /* Returns the square within the grid accociated with the coordinate */

            // retrieve rank from mapping the 2nd char to an integer
            int rank  = coord_str[1] - '0' - 1;
            // letter from first char
            char letter = coord_str[0];
            // find the correct fileumn index from letter
            int i;
            for (i = 0; i < 8; i++) 
                if (letter == letter_coordinates[i])
                    break;
            // return square with this id
            int id = i + rank * 8;
            return grid[id];

        };

        char get_symbol_from_coord (string coord_str) {

            /* Returns the occupation symbol i.e. chess piece at the 
            desired coordinate. The return '_' indicates an empty square. */

            return get_square_from_coord(coord_str)["symbol"][0];
        };

        int square_is_occupied (string coord_str) {
            return get_symbol_from_coord(coord_str) != '_';
        }

        int square_is_occupied_by_enemy (int friendly_color, string coord_str) {

            /* Returns boolean-like integer. The return is 1 if the square is occupied by enemy piece otherwise 0 */

            char target_symbol = get_symbol_from_coord(coord_str);
            int target_color = get_color_from_symbol(target_symbol); 

            return target_color != friendly_color && target_color != 0;

        }


        /* output methods */
        void print_board (int unicode = 0) {
            
            /* Prints the board in the terminal.
            If 'unicode' is enabled chess unicode symbols will be displayed
            otherwise alphabetical symbols. */
            
            // iterate through grid
            int id;
            string line;
            wcout << endl;
            for (int rank = 7; rank >= 0; rank--) {
                line = "";
                for (int file = 0; file < 8; file++) {
                    id = rank * 8 + file;
                    if (unicode) {
                        wcout << pieces.to_unicode(grid[id]["symbol"][0]).c_str();
                    } else {
                        wcout << grid[id]["symbol"][0];
                    }
                }
                wcout << endl;
            }
            wcout << endl;
        };

        void show_material () {
            cout << "white's material count: " << count_material(pieces.w) << endl;
            cout << "black's material count: " << count_material(pieces.b) << endl;
        };

        void show_reachable_squares (string coord_str) {
            
            /* Prints the reachable targets delimited into console */
            vector<string> targets = reachable_target_coords(coord_str);
            for (int i = 0; i < targets.size(); i++) {
                cout << targets[i] << " ";
            }
            cout << endl;
        }


        /* manipulation/set methods */
        void ignorant_move (string origin_coord_str, string target_coord_str) {

            /* Moves a piece disregarding chess rules by a combination of remove and place methods */

            // determine the piece color
            int color;
            char origin_symbol = get_square_from_coord(origin_coord_str)["symbol"][0];  // not proper
            if ( pieces.is_white(origin_symbol) ) 
                color = 8;
            else 
                color = 16;
            
            // determine piece
            int piece = pieces.from_symbol(origin_symbol);
            
            // remove the piece from origin
            remove_piece(origin_coord_str);

            // place the piece at new target
            place_piece(piece, color, target_coord_str);

        };

        void load_position_from_fen (string fen) {

            /* A fen parsing implementation which generates a position from compact string.
            The reading starts from upper left corner i.e. rank is decremented while the files
            are iterated from left to right. */
            
            int id;
            int piece;
            int color;
            int file = 1, rank = 8;
            int pieces_completely_parsed = 0;
            int active_color_parsed = 0;
            int castling_parsed = 0;
            int en_passant_parsed = 0;
            int half_clock_parsed = 0;
            int move_count_parsed = 0;
            char _char;
            char delimiter = '/';
            string coord;
            string moves = "";
            string castling_options = "KkQq-";

            
            // first exchange all slashes for unique symbols to avoid escapes
            int delimiter_index;
            while (delimiter_index != string::npos) {
                delimiter_index = fen.find(delimiter);
                fen[delimiter_index] = '&';
            }
            
            // delete global variables 
            en_passant_coord = "";
            moves = "";
            
            // parse ...
            for (int i = 0; i < fen.size(); i++) {
                
                _char = fen[i];
                //cout << "test y " << i << " " <<  _char << endl;

                // parse piece locations
                if (!pieces_completely_parsed) {

                    // determine piece and color from symbol char
                    piece = pieces.from_symbol(_char);
                    //cout << "test piece " << _char << " " << piece << endl;
                    
                    // if line break is parsed decrement rank
                    if (_char == '&') {
                        // decrement rank
                        rank--;
                        // reset pointer to A file
                        file = 1;
                        continue;
                    // if integer is parsed shift file
                    } else if (isdigit(_char)) {
                        // integers account for file shifts
                        file += _char - '0';
                        continue;
                    // finish parsing pieces on space
                    } else if (_char == ' ') {
                        pieces_completely_parsed = 1;
                        continue;
                    }
                    
                    // otherwise
                    if (pieces.is_white(_char)) {
                        color = 8;
                    } else {
                        color = 16;
                    }
                    
                    // determine the id from current rank and file pointer
                    id = file - 1 + 8 * (rank - 1); 
                    coord = get_coord_from_id(id);

                    // place the piece
                    place_piece(piece, color, coord);

                    // increment file
                    file++;
                
                // check for active color & override
                } else if (pieces_completely_parsed && !active_color_parsed && (_char == 'b' || _char == 'w')) {
                    active_color = _char;
                    active_color_parsed = 1;
                    file++;

                // check for castling rights 
                } else if (pieces_completely_parsed && active_color_parsed && !castling_parsed ) {
                    if (_char == ' ') {
                        castling_parsed = 1;
                    } else if (_char == 'K') {
                        castling_right_k_w = 1;
                    } else if (_char == 'Q') {
                        castling_right_q_w = 1;
                    } else if (_char == 'k') {
                        castling_right_k_b = 1;
                    } else if (_char == 'q') {
                        castling_right_q_b = 1;
                    }
                
                // check for en-passant availability
                } else if (castling_parsed && !en_passant_parsed) {
                    en_passant_coord += _char;
                    if (_char == '-' || en_passant_coord.size() >= 2) {
                        en_passant_parsed = 1;
                        i++;
                    }
                // parse and denote half clock count
                } else if (en_passant_parsed && !half_clock_parsed) {
                    if (_char == ' ') {
                        half_clock_parsed = 1;
                    } else {
                        half_clock += _char;
                    }
                    i++;
                // parse and denote the move count
                } else if (half_clock_parsed && !move_count_parsed) {
                    moves += _char;
                    if (i == fen.size()-1) {
                        cout << "test moves " << moves << endl;
                        move_count = stoi(moves);
                        cout << "Successfully loaded position from FEN." << endl;
                        // finished   
                    }
                } 

            };
        };
        
        void load_starting_position () {
            
            /* Loads all pieces to the board grid by using their 
            symbol values. Start position fen is loaded via fen parser.*/

            cout << "load starting position ..." << endl;
            load_position_from_fen(starting_position_fen);
            cout << "successfully loaded starting position." << endl;
        };

        void place_piece (int piece, int color, string coord_str) {

            /* Places a piece on the board */
            char piece_symbol = pieces.to_symbol(piece, color);

            if (__verbose__)
                cout << "place " << pieces.name_from_symbol(piece_symbol) << " (" << piece_symbol << ")" << " at " << coord_str << endl;
            
            set_symbol_at_coord(piece_symbol, coord_str);

        };

        void remove_piece (string coord_str) {

            /* Removes a piece from requested coordinate */

            char piece_symbol = get_symbol_from_coord(coord_str);

            if (__verbose__)
                cout << "remove " << pieces.name_from_symbol(piece_symbol) << " (" << piece_symbol << ")" << " at " << coord_str << endl;
            
            // override square value with underscore
            set_symbol_at_coord('_', coord_str);

        };

        void set_symbol_at_coord (char symbol, string coord_str) {

            /* Overrides the symbol at square with respecting coordinate */

            // retrieve rank from mapping the 2nd char to an integer
            int rank  = coord_str[1] - '0' - 1;
            // letter from first char
            char letter = coord_str[0];
            // find the correct fileumn index from letter
            int i;
            for (i = 0; i < 8; i++) {
                if (letter == letter_coordinates[i]) {
                    break;
                }
            }
            int id = i + rank * 8;
            // override the symbol value
            grid[id]["symbol"] = symbol;

        };


        
    private:

        /*   Define global variables   */
        // prepare letters for the board fileumns 
        const char letter_coordinates[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
        const string starting_position_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        char active_color = 'w';

        // castling rights
        int castling_right_k_w = 1;
        int castling_right_k_b = 1;
        int castling_right_q_w = 1;
        int castling_right_q_b = 1;

        string en_passant_coord = "-";
        int half_clock = 0;
        int move_count = 1;

        // The board grid maps the square IDs (0 ... 63) => square info.*/
        map<string, string> grid[64] = {};

        // Load the pieces
        Piece pieces;

        /*   init methods   */
        void build_grid () {

            /* We will numerate each square by an array index 0-63 
            always starting from white's side i.e. A1 coordinate.
            Each array element maps to the corr. square information.  
            
            id      0   1   2   ... 62  63
            coord   A1  A2  A3  ... H7  H8    
            rank    0   1   2   ... 6   7
            file    0   0   0   ... 7   7
            object  map map map ... map map
                     |
                   symbol
                square color
                    rank
                    file
                    etc.
            */
            
            int rank, file;
            string coord;
            
            for (int i = 0; i < 64; i++) {
                
                // track rank and file
                rank = (int)( i * 0.125 + 1 );
                file = i % 8;

                // construct the coordinate by concatenating rank number and fileumn letter
                coord = letter_coordinates[file] + to_string(rank);
                //cout << "rank " << rank << " " << coord << endl; // for testing

                // fill the info to the map at id "i"
                grid[i]["id"] = to_string(i);
                grid[i]["coordinate"] = coord;
                grid[i]["rank"] = to_string(rank-1);
                grid[i]["file"] = to_string(file);  
                grid[i]["symbol"] = "_";
                grid[i]["is_light_square"] = (file + rank) % 2 != 0; 
            }

        };
        

        /* chess rules and logic */
        vector<string> reachable_target_coords (string coord_str) {

            /* This method is the main part of move interpretation
            of a single piece at a coord. An array of reachable coordinates is returned, 
            if the square is occupied by a friendly piece which has a possible
            square to move, otherwise an empty array is returned surely. 
            The reachable targets are not necessary legal moves. 
            
            Left to be added:
            - castling options
            - en-passant options
            
            */

            vector<string> out = {};
            map <string, string> square = get_square_from_coord(coord_str);
            char symbol = square["symbol"][0];
            
            // cut short in case that the square is empty
            if (symbol == '_') {return out;}

            
            int color = get_color_from_symbol(symbol);
            int piece = pieces.from_symbol(symbol);

            // cout << "test 1 rank and file " << square["rank"] << " " << square["file"] << endl; 

            string target_coord;
            int rank = stoi(square["rank"]),
                file = stoi(square["file"]); 

            
            // cout << "test 2 rank and file " << rank << " " << file << endl; 
            // cout << "test symbol " << symbol << endl;  
            

            // piece-dep. decision tree
            if (piece == pieces.Pawn) {

                // if white is playing
                if (color == 8) {

                    cout << "test color white" << endl;

                    // check if forward-left captures is possible
                    if (file-1 >= 0 && rank + 1 < 8) {
                        target_coord = get_coord_from_file_and_rank(file-1, rank+1);
                        if (square_is_occupied_by_enemy(color, target_coord)) {
                            out.push_back(target_coord);
                        }
                    }

                    // forward-right captures
                    if (file + 1 < 8 && rank + 1 < 8) {
                        target_coord = get_coord_from_file_and_rank(file+1, rank+1);
                        if (square_is_occupied_by_enemy(color, target_coord)) {
                            cout << "test captures" << endl;
                            out.push_back(target_coord);
                        }
                    }
                    
                    // pushing forward
                    // select the amount of steps a pawn can make dep. on starting rank
                    int steps = 1;
                    if (rank == 1) {steps = 2;}
                    // iterate possibilities
                    for (int step = 1; step < steps + 1; step++) {
                        if (rank + step < 8) {
                            target_coord = get_coord_from_file_and_rank(file, rank + step);
                            cout << "test target " << target_coord << endl;
                            if (!square_is_occupied(target_coord)) {
                                out.push_back(target_coord);
                            }
                        }
                    }
                    

                    // en-passant possibility

                } else if (color == 16) {

                    // check if forward-left captures is possible
                    if (file+1 < 8 && rank - 1 >= 0) {
                        target_coord = get_coord_from_file_and_rank(file-1, rank - 1);
                        if (square_is_occupied_by_enemy(color, target_coord)) {
                            out.push_back(target_coord);
                        }
                    }

                    // forward-right captures
                    if (file - 1 >= 0 && rank - 1 >= 0) {
                        target_coord = get_coord_from_file_and_rank(file - 1, rank - 1);
                        if (square_is_occupied_by_enemy(color, target_coord)) {
                            out.push_back(target_coord);
                        }
                    }
                    
                    // pushing forward
                    // select the amount of steps a pawn can make dep. on starting rank
                    int steps = 1;
                    if (rank == 6) {steps = 2;}
                    // iterate possibilities
                    for (int step = 1; step < steps + 1; step++) {
                        if (rank - step >=0 ) {
                            target_coord = get_coord_from_file_and_rank(file, rank - step);
                            if (!square_is_occupied(target_coord)) {
                                out.push_back(target_coord);
                            }
                        }
                    }

                    // en-passant possibility

                }
                

            } else if (piece == pieces.Knight) {

                // front 2 left 1
                if (file - 1 >= 0 && rank + 2 <= 7) {
                    target_coord = get_coord_from_file_and_rank(file-1, rank+2);
                    if (!square_is_occupied(target_coord) || (square_is_occupied(target_coord) && square_is_occupied_by_enemy(color, coord_str))) {
                        out.push_back(target_coord);
                    }
                }
                
                // front 1 left 2
                if (file - 2 >= 0 && rank + 1 <= 7) {
                    target_coord = get_coord_from_file_and_rank(file-1, rank+2);
                    if (!square_is_occupied(target_coord) || (square_is_occupied(target_coord) && square_is_occupied_by_enemy(color, coord_str))) {
                        out.push_back(target_coord);
                    }
                } 

                // front 2 right 1
                if (file + 1 < 8 && rank + 2 < 8) {
                    target_coord = get_coord_from_file_and_rank(file + 1, rank + 2);
                    if (!square_is_occupied(target_coord) || (square_is_occupied(target_coord) && square_is_occupied_by_enemy(color, coord_str))) {
                        out.push_back(target_coord);
                    }
                } 

                // front 1 right 2
                if (file + 2 >= 0 && rank + 1 < 8) {
                    target_coord = get_coord_from_file_and_rank(file + 2, rank + 1);
                    if (!square_is_occupied(target_coord) || (square_is_occupied(target_coord) && square_is_occupied_by_enemy(color, coord_str))) {
                        out.push_back(target_coord);
                    }
                } 

                // back 2 left 1
                if (file - 1 >= 0 && rank - 2 >= 0) {
                    target_coord = get_coord_from_file_and_rank(file - 1, rank - 2);
                    if (!square_is_occupied(target_coord) || (square_is_occupied(target_coord) && square_is_occupied_by_enemy(color, coord_str))) {
                        out.push_back(target_coord);
                    }
                } 

                // back 1 left 2
                if (file - 2 >= 0 && rank - 1 >= 0) {
                    target_coord = get_coord_from_file_and_rank(file - 2, rank - 1);
                    if (!square_is_occupied(target_coord) || (square_is_occupied(target_coord) && square_is_occupied_by_enemy(color, coord_str))) {
                        out.push_back(target_coord);
                    }
                } 

                // back 2 right 1
                if (file + 1 < 8 && rank - 2 >= 0) {
                    target_coord = get_coord_from_file_and_rank(file + 1, rank - 2);
                    if (!square_is_occupied(target_coord) || (square_is_occupied(target_coord) && square_is_occupied_by_enemy(color, coord_str))) {
                        out.push_back(target_coord);
                    }
                } 

                // back 1 right 2
                if (file + 2 < 8 && rank - 1 >= 0) {
                    target_coord = get_coord_from_file_and_rank(file + 2, rank - 1);
                    if (!square_is_occupied(target_coord) || (square_is_occupied(target_coord) && square_is_occupied_by_enemy(color, coord_str))) {
                        out.push_back(target_coord);
                    }
                } 

            } else if (piece == pieces.Bishop) {

                // foward right direction
                for (int i = 1; i < 8; i++) {
                    if (file + i < 8 && rank + i < 8) {
                        target_coord = get_coord_from_file_and_rank(file+i, rank+i);
                        // square is free
                        if (!square_is_occupied(target_coord)) {
                            out.push_back(target_coord);
                        } 
                        // capture
                        else if (square_is_occupied_by_enemy(color, coord_str)) {
                            out.push_back(target_coord);
                            break;
                        } 
                        // square is blocked by friendly piece
                        else {
                            break;
                        }
                    } else {
                        break;
                    }
                }
                // foward left direction
                for (int i = 1; i < 8; i++) {
                    if (file - i >= 0 && rank + i < 8) {
                        target_coord = get_coord_from_file_and_rank(file - i, rank+i);
                        // square is free
                        if (!square_is_occupied(target_coord)) {
                            out.push_back(target_coord);
                        } 
                        // capture
                        else if (square_is_occupied_by_enemy(color, coord_str)) {
                            out.push_back(target_coord);
                            break;
                        } else {
                            break;
                        }
                    } 
                    // square is blocked by friendly piece
                    else {
                        break;
                    }
                }
                // backward right direction
                for (int i = 1; i < 8; i++) {
                    if (file + i < 8 && rank - i >= 0) {
                        target_coord = get_coord_from_file_and_rank(file + i, rank - i);
                        // square is free
                        if (!square_is_occupied(target_coord)) {
                            out.push_back(target_coord);
                        } 
                        // capture
                        else if (square_is_occupied_by_enemy(color, coord_str)) {
                            out.push_back(target_coord);
                            break;
                        } 
                        // square is blocked by friendly piece
                        else {
                            break;
                        }
                    } else {
                        break;
                    }
                }
                // backward left direction
                for (int i = 1; i < 8; i++) {
                    if (file - i >= 0 && rank - i >= 0) {
                        target_coord = get_coord_from_file_and_rank(file - i, rank - i);
                        // square is free
                        if (!square_is_occupied(target_coord)) {
                            out.push_back(target_coord);
                        } 
                        // capture
                        else if (square_is_occupied_by_enemy(color, coord_str)) {
                            out.push_back(target_coord);
                            break;
                        } 
                        // square is blocked by friendly piece
                        else {
                            break;
                        }
                    } else {
                        break;
                    }
                }
            } else if (piece == pieces.Rook) {

                // forward
                for (int i = 1; i < 8; i++) {
                    if (rank + i < 8) {
                        target_coord = get_coord_from_file_and_rank(file, rank+i);
                        if (!square_is_occupied(target_coord)) {
                            out.push_back(target_coord);
                        } else if (square_is_occupied(target_coord) && square_is_occupied_by_enemy(color, coord_str)) {
                            out.push_back(target_coord);
                            break;
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }

                // backward
                for (int i = 1; i < 8; i++) {
                    if (rank - i >= 0) {
                        target_coord = get_coord_from_file_and_rank(file, rank - i);
                        if (!square_is_occupied(target_coord)) {
                            out.push_back(target_coord);
                        } else if (square_is_occupied(target_coord) && square_is_occupied_by_enemy(color, coord_str)) {
                            out.push_back(target_coord);
                            break;
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }

                // left
                for (int i = 1; i < 8; i++) {
                    if (file - i >= 0) {
                        target_coord = get_coord_from_file_and_rank(file - i, rank);
                        if (!square_is_occupied(target_coord)) {
                            out.push_back(target_coord);
                        } else if (square_is_occupied(target_coord) && square_is_occupied_by_enemy(color, coord_str)) {
                            out.push_back(target_coord);
                            break;
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }

                // right
                for (int i = 1; i < 8; i++) {
                    if (file + i < 8) {
                        target_coord = get_coord_from_file_and_rank(file + i, rank);
                        if (!square_is_occupied(target_coord)) {
                            out.push_back(target_coord);
                        } else if (square_is_occupied(target_coord) && square_is_occupied_by_enemy(color, coord_str)) {
                            out.push_back(target_coord);
                            break;
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }
                
            } else if (piece == pieces.Queen) {

                // the first four straight directions are analogous to the rook
                // forward
                for (int i = 1; i < 8; i++) {
                    if (rank + i < 8) {
                        target_coord = get_coord_from_file_and_rank(file, rank+i);
                        if (!square_is_occupied(target_coord)) {
                            out.push_back(target_coord);
                        } else if (square_is_occupied(target_coord) && square_is_occupied_by_enemy(color, coord_str)) {
                            out.push_back(target_coord);
                            break;
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }

                // backward
                for (int i = 1; i < 8; i++) {
                    if (rank - i >= 0) {
                        target_coord = get_coord_from_file_and_rank(file, rank - i);
                        if (!square_is_occupied(target_coord)) {
                            out.push_back(target_coord);
                        } else if (square_is_occupied(target_coord) && square_is_occupied_by_enemy(color, coord_str)) {
                            out.push_back(target_coord);
                            break;
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }

                // left
                for (int i = 1; i < 8; i++) {
                    if (file - i >= 0) {
                        target_coord = get_coord_from_file_and_rank(file - i, rank);
                        if (!square_is_occupied(target_coord)) {
                            out.push_back(target_coord);
                        } else if (square_is_occupied(target_coord) && square_is_occupied_by_enemy(color, coord_str)) {
                            out.push_back(target_coord);
                            break;
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }

                // right
                for (int i = 1; i < 8; i++) {
                    if (file + i < 8) {
                        target_coord = get_coord_from_file_and_rank(file + i, rank);
                        if (!square_is_occupied(target_coord)) {
                            out.push_back(target_coord);
                        } else if (square_is_occupied(target_coord) && square_is_occupied_by_enemy(color, coord_str)) {
                            out.push_back(target_coord);
                            break;
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }

                // the diagonal directions are analogous to the bishop
                // foward right direction
                for (int i = 1; i < 8; i++) {
                    if (file + i < 8 && rank + i < 8) {
                        target_coord = get_coord_from_file_and_rank(file+i, rank+i);
                        // square is free
                        if (!square_is_occupied(target_coord)) {
                            out.push_back(target_coord);
                        } 
                        // capture
                        else if (square_is_occupied_by_enemy(color, coord_str)) {
                            out.push_back(target_coord);
                            break;
                        } 
                        // square is blocked by friendly piece
                        else {
                            break;
                        }
                    } else {
                        break;
                    }
                }
                // foward left direction
                for (int i = 1; i < 8; i++) {
                    if (file - i >= 0 && rank + i < 8) {
                        target_coord = get_coord_from_file_and_rank(file - i, rank+i);
                        // square is free
                        if (!square_is_occupied(target_coord)) {
                            out.push_back(target_coord);
                        } 
                        // capture
                        else if (square_is_occupied_by_enemy(color, coord_str)) {
                            out.push_back(target_coord);
                            break;
                        } else {
                            break;
                        }
                    } 
                    // square is blocked by friendly piece
                    else {
                        break;
                    }
                }
                // backward right direction
                for (int i = 1; i < 8; i++) {
                    if (file + i < 8 && rank - i >= 0) {
                        target_coord = get_coord_from_file_and_rank(file + i, rank - i);
                        // square is free
                        if (!square_is_occupied(target_coord)) {
                            out.push_back(target_coord);
                        } 
                        // capture
                        else if (square_is_occupied_by_enemy(color, coord_str)) {
                            out.push_back(target_coord);
                            break;
                        } 
                        // square is blocked by friendly piece
                        else {
                            break;
                        }
                    } else {
                        break;
                    }
                }
                // backward left direction
                for (int i = 1; i < 8; i++) {
                    if (file - i >= 0 && rank - i >= 0) {
                        target_coord = get_coord_from_file_and_rank(file - i, rank - i);
                        // square is free
                        if (!square_is_occupied(target_coord)) {
                            out.push_back(target_coord);
                        } 
                        // capture
                        else if (square_is_occupied_by_enemy(color, coord_str)) {
                            out.push_back(target_coord);
                            break;
                        } 
                        // square is blocked by friendly piece
                        else {
                            break;
                        }
                    } else {
                        break;
                    }
                }
            } else if (piece == pieces.King) {

                // iterate through neighbourhood
                for (int i = -1; i < 2; i++) {
                    for (int j = -1; j < 2; j++) {
                        // skip origin square
                        if ( !(i == 0 && j == 0) && square_is_valid(rank+i, file+j) ) {
                            target_coord = get_coord_from_file_and_rank(rank+i, file+j);
                            if (!square_is_occupied(target_coord) || square_is_occupied_by_enemy(color, target_coord)) {
                                out.push_back(target_coord);
                            }
                        }
                    }
                }

                // castling
                
            }
            
            return out;
        };
        
        vector<string> scoping_enemy_squares (string coord_str) {
            
            /* A function which returns all enemy squares whose direction points to a demanded square. 
            Regardless of wether a piece is blocking it. */

            vector<string> out = {};
            map <string, string> square = get_square_from_coord(coord_str);
            char symbol = square["symbol"][0];
            
            int color = get_color_from_symbol(symbol);

            // cout << "test 1 rank and file " << square["rank"] << " " << square["file"] << endl; 

            string target_coord;
            char target_symbol;
            int piece,
                rank = stoi(square["rank"]),
                file = stoi(square["file"]); 

            // check diagonals for enemy queen or bishop 
            for (int i = -7; i < 8; i++) {
                if (square_is_valid(rank+i, file+i)) {
                    target_coord = get_coord_from_file_and_rank(rank+i, file+i);
                    target_symbol = get_symbol_from_coord(target_coord);
                    piece = pieces.from_symbol(target_symbol);
                    if (square_is_occupied_by_enemy(color, target_coord) && ( piece == pieces.Queen || piece == pieces.Bishop) ) {
                        out.push_back(target_coord);
                    }
                }
                if (square_is_valid(rank-i, file+i)) {
                    target_coord = get_coord_from_file_and_rank(rank-i, file+i);
                    target_symbol = get_symbol_from_coord(target_coord);
                    piece = pieces.from_symbol(target_symbol);
                    if (square_is_occupied_by_enemy(color, target_coord) && ( piece == pieces.Queen || piece == pieces.Bishop) ) {
                        out.push_back(target_coord);
                    }
                }
            }

            // check straight lines for enemy rook or queen 
            for (int i = -7; i < 8; i++) {
                if (square_is_valid(rank+i, file)) {
                    target_coord = get_coord_from_file_and_rank(rank+i, file+i);
                    target_symbol = get_symbol_from_coord(target_coord);
                    piece = pieces.from_symbol(target_symbol);
                    if (square_is_occupied_by_enemy(color, target_coord) && ( piece == pieces.Queen || piece == pieces.Bishop) ) {
                        out.push_back(target_coord);
                    }
                }
                if (square_is_valid(rank, file+i)) {
                    target_coord = get_coord_from_file_and_rank(rank-i, file+i);
                    target_symbol = get_symbol_from_coord(target_coord);
                    piece = pieces.from_symbol(target_symbol);
                    if (square_is_occupied_by_enemy(color, target_coord) && ( piece == pieces.Queen || piece == pieces.Rook) ) {
                        out.push_back(target_coord);
                    }
                }
            }
            
            return out;

        };

        int square_is_in_line (string coord1, string coord2, string probe_coord) {

            /* Checks quickly if a probing coordinate lies within the aligned line (vert.,hor.,diag.) 
            spanned by the first two coordinates. If the first two coordinates are not mutually aligned 
            or they are not aligned with the probing coordinate, the return will be 0. */

            int r1 = stoi(get_square_from_coord(coord1)["rank"]),
                r2 = stoi(get_square_from_coord(coord2)["rank"]),
                r3 = stoi(get_square_from_coord(probe_coord)["rank"]),
                f1 = stoi(get_square_from_coord(coord1)["file"]),
                f2 = stoi(get_square_from_coord(coord2)["file"]),
                f3 = stoi(get_square_from_coord(probe_coord)["file"]);
            
            // all pieces are aligned horizontally
            if (r1 == r2 && r3 == r1) {
                if (f1 > f2 && f1 > f3 && f3 > f2 || f2 > f1 && f3 > f1 && f2 > f3)
                    return 1;
            } 
            // all pieces are aligned vertically
            else if (f1 == f2 && f3 == f1) {
                if ( (r1 > r2 && r1 > r3 && r3 > r2) || (r2 > r1 && r2 > r3 && r3 > r1)) 
                    return 1;
            } 
            // all pieces are aligned diagonally
            else if (abs(f1-f2) == abs(r1-r2) && abs(r1-r3) == abs(f1-f3) && (r1-r2)/abs(r1-r2) == abs(r1-r3)/abs(r1-r3) && (f1-f2)/abs(f1-f2) == abs(f1-f3)/abs(f1-f3)) {
                if ( !(f1 > f2 && f1 > f3 && f3 > f2) || !(f2 > f1 && f2 > f3 && f3 > f1) ) 
                    return 0;
                if ((r1 > r2 && r1 > r3 && r3 > r2) || (r2 > r1 && r2 > r3 && r3 > r1) ) 
                    return 1;
            }

            return 0;
        };

        int square_is_valid (int rank, int file) {

            /* Checks if the rank and file integer provided map to a valid chess square. */

            // simply check if the indices lie within the boundaries
            if (rank >= 0 && rank < 8 && file >= 0 && file < 8)
                return 1;
            else
                return 0;
            
        };

        int squares_are_aligned (string coord1, string coord2) {
            /* Check if the squares are aligned in any way. */
            if (squares_are_aligned_diagonally(coord1, coord2) || squares_are_aligned_horizontally(coord1, coord2) || squares_are_aligned_vertically(coord1, coord2))
                return 1;
            return 0;
        }

        int squares_are_aligned_diagonally (string coord1, string coord2) {
            int r1 = stoi(get_square_from_coord(coord1)["rank"]);
            int r2 = stoi(get_square_from_coord(coord2)["rank"]);
            int f1 = stoi(get_square_from_coord(coord1)["file"]);
            int f2 = stoi(get_square_from_coord(coord2)["file"]);
            if (abs(r1-r2) == abs(f1-f2))
                return 1;
            return 0;
        };

        int squares_are_aligned_horizontally (string coord1, string coord2) {
            int r1 = stoi(get_square_from_coord(coord1)["rank"]);
            int r2 = stoi(get_square_from_coord(coord2)["rank"]);
            if (r1 == r2)
                return 1;
            return 0;
        };

        int squares_are_aligned_vertically (string coord1, string coord2) {
            int f1 = stoi(get_square_from_coord(coord1)["file"]);
            int f2 = stoi(get_square_from_coord(coord2)["file"]);
            if (f1 == f2)
                return 1;
            return 0;
        }

        /* Evaluation */
        int count_material (int color) {

            /* Counts material for specific color */

            char symbol;
            int piece;
            int white;
            int value = 0;

            for (int i = 0; i < 64; i++) {
                symbol = grid[i]["symbol"][0];
                white = pieces.is_white(symbol);
                if (white && color == 8 || !white && color == 16 )
                    value = value + pieces.value_from_symbol(symbol);
            }

            return value;
        }
        
        float position_activity (char symbol, string coord_str) {

            /* Returns the positional activity derived from a map for each peace. */

            int piece = pieces.from_symbol(symbol);
            int rank = stoi(get_square_from_coord(coord_str)["rank"]),
                file = stoi(get_square_from_coord(coord_str)["file"]);

            if (pieces.is_white(symbol))
                return pieces.positional_weight_map[piece-1][7-rank][file];
            else
                return pieces.positional_weight_map[piece-1][rank][7-file];
        };
};



int main (void) {

    // set output mode for unicode printing
    //_setmode(_fileno(stdout), _O_WTEXT);

    // initialize a new board and pieces objects
    Piece pieces;
    Board boardObject;
    
    // int id = 0;
    // cout << "id test " << id << " " << boardObject.get_coord_from_id(id);
    boardObject.load_starting_position();
    
    boardObject.print_board();
    boardObject.ignorant_move("E2", "E4");
    boardObject.ignorant_move("D7", "D5");
    boardObject.show_reachable_squares("E4");
    boardObject.show_material();
    boardObject.print_board();
    
    return 0;
}