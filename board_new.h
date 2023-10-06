#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cmath>

// include objects and libraries
#include "moveInfo.h"
#include "piece.h"
#include "general.h"
#include "snapshot.h"

using namespace std;



/* define method for call statistics
this will trigger in any function if __TESTING__is enabled.*/ 
bool __TESTING__ = false;
std::map<const string, int> counts_;
void __call__ (const string function_name) { if (__TESTING__) ++counts_[function_name]; } 



/*  Chess Board Implementation */
class Board {   

    /*
    Chess board implementation with internal Pieces instance.
    
    - Comes with integrated pieces objects.
    - Provides move mechanics, with an undo depth of 1
    - Counts possible squares, checks, legality and other chess rules.
    - Can parse and output positions in FEN or PGN format 
      (PGN move reversibility will be handled by the engine)
    - denotes move information and clock

    ==== Call Statistics ====
    active_move: 3
    can_castle_king_side: 7
    can_castle_queen_side: 7
    get_color_from_symbol: 540
    get_coord_from_file_and_rank: 388
    get_coord_from_id: 420
    get_square_from_coord: 1106
    get_symbol_from_coord: 985
    ignorant_move: 86
    is_checked: 100
    legal_move: 3
    load_position_from_fen: 1
    load_starting_position: 1
    move_is_legal: 3
    move_leaves_open_check: 83
    move_to_pgn: 3
    place_piece: 118
    place_symbol: 84
    reachable_target_coords: 112
    refresh_position: 3
    remove_piece: 169
    set_symbol_at_coord: 371
    show_active_color: 1
    show_board: 4
    square_is_attacked: 100
    inside_bounds: 1042
    square_is_occupied: 299
    square_is_occupied_by_enemy: 342
    undo_ignorant_move: 83
    update_moves_from_targets: 4
    update_reachable_target_map: 7

    */

    public:

        /* ---- General Parameters ---- */

        // Verbose mode which prints every operation
        bool __verbose__ = true; 


        /* ---- declare global variables ---- */

        

        
         

        // The board grid maps the square IDs (0 ... 63) => square info.*/
        map<string, string> grid[64];

        

        // Init game parameters
        int active_color = pieces.w;

        /* while the last_move object is for quick iteration this  
        will be empty after position refresh
        while the persistent object remains. */ 
        string last_move[2] = {"", ""};
        string last_move_persistent[2] = {"", ""};
        char last_captured_symbol = '_';
        string last_captured_symbol_coord = "";
        

        // fen parameters
        string en_passant_coord = "-";
        string en_passant_was_on = "-";
        int half_clock = 0;
        




        /* ---- get methods ---- */

        // int get_color_from_symbol (char symbol) {

        //     /* Returns the piece color int from symbol char e.g. k -> 16, Q -> 8
        //     where 16 is black and 8 white. If the symbol is None '_' the return will be 0. */

        //     __call__(__func__);
        //     if (pieces.is_white(symbol))
        //         return 8;
        //     else if (symbol == '_')
        //         return 0;
        //     else
        //         return 16; 

        // };

        // string get_coord_from_id (int id) {

        //     /* Returns the string coordinate for grid id (0-63). */

        //     __call__(__func__);
        //     return grid[id]["coordinate"];

        // };

        // string get_coord_from_file_and_rank (int file, int rank) {
            
        //     /* Returns the coordinate string for provided rank and file integers.
        //     e.g. (0,0) -> A1 or (0,1) -> A2 etc.*/

        //     __call__(__func__);
        //     // return get_coord_from_id(file + 8 * rank);
        //     return letter_coordinates[file] + to_string(rank+1); // faster method

        // };

        // int get_id_from_coord (string coord_str) {

        //     /* Returns the id assigned to the square at req. coordinate */
            
        //     __call__(__func__);
        //     return stoi(get_square_from_coord(coord_str)["id"]);

        // };


        // map<string, string> get_square_from_coord (string coord_str) {

        //     /* Returns the square within the grid accociated with the coordinate */

        //     __call__(__func__);

        //     // retrieve rank from mapping the 2nd char to 
        //     // an integer.
        //     int rank  = coord_str[1] - '0' - 1;
            
        //     // letter from first char
        //     char letter = coord_str[0];
            
        //     // find the correct fileumn index from letter
        //     int i;
        //     for (i = 0; i < 8; i++) 
        //         if (letter == letter_coordinates[i])
        //             break;
            
        //     // return square with this id
        //     return grid[i + rank * 8];

        // };

        // char get_symbol_from_coord (string coord_str) {

        //     /* Returns the occupation symbol i.e. chess piece at the 
        //     desired coordinate. The return '_' indicates an empty square. */
            
        //     __call__(__func__);
        //     return get_square_from_coord(coord_str)["symbol"][0];

        // };
        

        /* ---- output methods ---- */
        
        void show_active_color () {
            
            /* Returns the color which has to move. */

            string col;
            if (active_color == pieces.w) 
                col = "white";
            else
                col = "black";
            cout << "It's " << col << "'s move." << endl;

        };

        void show_board (int unicode = 0) {
            
            /* Prints the board in the terminal.
            If 'unicode' is enabled chess unicode symbols will be displayed
            otherwise alphabetical symbols. */
            
            // iterate through grid
            int id;
            string line;
            cout << endl;
            for (int rank = 7; rank >= 0; rank--) {
                line = "";
                for (int file = 0; file < 8; file++) {
                    id = rank * 8 + file;
                    cout << grid[id]["symbol"][0];
                }
                cout << endl;
            }
            cout << endl;

        };

        void show_castling_rights () {
            
            cout << "castling rights:" << 
            castling_right_k_w << " " <<
            castling_right_k_b << " " <<
            castling_right_q_w << " " <<
            castling_right_q_b << " " << endl;

        };
        
        void show_en_passant () {
            
            cout << "En passant coordinate: " << en_passant_coord << endl;

        };

        void show_half_clock () {

            cout << "half clock: " << half_clock << endl;

        };

        void show_move_count () {
            
            cout << "move count: " << move_count << endl;

        };

        void show_call_stats () {

            string func_name;
            int calls;
            
            cout << "==== Call Statistics ====" << endl;

            for (auto const& x : counts_) {

                func_name = x.first;
                calls = x.second;
                
                cout << func_name << ": " << calls << endl;

            }

        }

        void show_moves_for_active_color () {
            
            string  col_str,
                    target_string,
                    coord;
            vector<string> targets;
            map<string, vector<string>> m;

            if (active_color == pieces.w) {
                col_str = "white";
                m = white_moves;
            } else {
                m = black_moves;
                col_str = "black";
            }

            cout << "moves for " << col_str << ":" << endl;
            for (auto const& x : m) {

                coord = x.first;
                targets = x.second;

                target_string = "";

                for (int i = 0; i < targets.size(); i++) {   
                    target_string += targets[i] + " ";
                }
                
                cout << get_symbol_from_coord(coord) << " (" << coord << "): " << target_string << endl;

            }

        }

        void show_pgn () {

            /* Outputs the PGN code to console. */
            
            for (auto const& x : pgn_history) {
                cout << x.first << ". " << x.second[0];
                if (x.second.size() > 1)
                    cout << " " << x.second[1] << endl; 
                else
                    cout << endl; 
            }
                
        }

        void show_reachable_squares (string coord_str) {
            
            /* Prints the reachable targets delimited into console */

            vector<string> targets = reachable_target_coords(coord_str);
            for (int i = 0; i < targets.size(); i++) {
                cout << get_symbol_from_coord(coord_str) << " (" << coord_str << "): " << targets[i] << " " << endl;
            }

        };

        void show_reachable_squares_for_active_color () {

            /* Prints the reachable square coordinates for all pieces of active color. */

            string  col_str,
                    target_string,
                    coord;
            vector<string> targets;
            map<string, vector<string>> m;

            if (active_color == pieces.w) {
                col_str = "white";
                m = white_targets;
            } else {
                m = black_targets;
                col_str = "black";
            }

            cout << "reachable squares for " << col_str << ":" << endl;

            for (auto const& x : m) {

                coord = x.first;
                // show the square for the piece on coord
                show_reachable_squares (coord);

            }

        }


        /* ---- board manupulation methods ---- */

        void clear () {

            /* Removes all pieces from the board. */

            for (int i = 0; i < 64; i++)
                grid[i]["symbol"] = '_';
            
        };

        void load_position_from_fen (string fen, bool verbose=1) {

            /* A fen parsing implementation which generates a position from compact string.
            The reading starts from upper left corner i.e. rank is decremented while the files
            are iterated from left to right. */
            
            __call__(__func__);

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
            castling_right_k_w = 0;
            castling_right_k_b = 0;
            castling_right_q_w = 0;
            castling_right_q_b = 0;
            
            // parse ...
            for (int i = 0; i < fen.size(); i++) {
                
                _char = fen[i];

                // parse piece locations
                if (!pieces_completely_parsed) {

                    // determine piece and color from symbol char
                    piece = pieces.from_symbol(_char);
                    
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
                    
                    // otherwise _char is definitely a piece symbol
                    if (pieces.is_white(_char)) {
                        color = 8;
                    } else {
                        color = 16;
                    }
                    
                    // determine the id from current rank and file pointer
                    id = file - 1 + 8 * (rank - 1); 
                    coord = get_coord_from_id(id);
                    // coord = get_coord_from_file_and_rank(file, rank);

                    // place the piece
                    place_piece(piece, color, coord, verbose);

                    // override king position
                    if (_char == 'k')
                        black_king_coord = coord;
                    else if (_char == 'K')
                        white_king_coord = coord;

                    // increment file
                    file++;
                
                // check for active color & override
                } else if (pieces_completely_parsed && !active_color_parsed && (_char == 'b' || _char == 'w')) {
                    if (_char == 'w')
                        active_color = pieces.w;
                    else
                        active_color = pieces.b;
                    active_color_parsed = 1;
                    i++;

                // check for castling rights 
                } else if (pieces_completely_parsed && active_color_parsed && !castling_parsed ) {

                    if (_char == ' ') {
                        castling_parsed = 1;
                    } else if (_char == '-') {
                        castling_parsed = 1;
                        i++;
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
                        move_count = stoi(moves);
                        if (verbose)
                            print("Successfully loaded position from FEN.", "Board");
                        // finished   
                    }
                } 

            };

            // initialize all possible moves for white
            update_moves_from_targets(update_reachable_target_map(active_color), active_color);
            
        };
        
        void load_starting_position (bool verbose=1) {
            
            /* Loads all pieces to the board grid by using their 
            symbol values. Start position fen is loaded via fen parser.*/

            __call__(__func__);

            if (verbose)
                cout << "load starting position ..." << endl;

            // load position from FEN code    
            load_position_from_fen(starting_position_fen, verbose);

            if (verbose)
                cout << "successfully loaded starting position." << endl;

        };

        bool mate (int color) {
            __call__(__func__);
            return is_mate(color);
        }

        void place_piece (int piece, int color, string coord_str, bool verbose=1) {

            /* Places a piece on the board */

            __call__(__func__);
            char piece_symbol = pieces.to_symbol(piece, color);

            if (verbose)
                cout << "place " << pieces.name_from_symbol(piece_symbol) << " (" << piece_symbol << ")" << " at " << coord_str << endl;
            
            set_symbol_at_coord(piece_symbol, coord_str);

        };

        void place_symbol (char symbol, string coord_str) {

            /* Just an alias for symbol placement. */

            __call__(__func__);
            set_symbol_at_coord(symbol, coord_str);

        };

        void reset () {

            /* Resets the board by re-loading the starting position. */

            clear();
            load_starting_position();

        };

        void remove_piece (string coord_str, bool verbose=1) {

            /* Removes a piece from requested coordinate */

            __call__(__func__);
            char piece_symbol = get_symbol_from_coord(coord_str);
            if (verbose) {
                cout << "remove " << pieces.name_from_symbol(piece_symbol) << " (" << piece_symbol << ")" << " at " << coord_str << endl;
            }

            // override square value with underscore
            set_symbol_at_coord('_', coord_str);

            // addition (testing occupation map)
            if (pieces.is_white(piece_symbol))
                white_symbol_occupation_map.erase(coord_str);
            else
                black_symbol_occupation_map.erase(coord_str);

        };

        void set_symbol_at_coord (char symbol, string coord_str) {

            /* Overrides the symbol at square with respecting coordinate */

            __call__(__func__);

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

            // addition (testing occupation map)
            if (symbol != '_')
                if (pieces.is_white(symbol))
                    white_symbol_occupation_map[coord_str] = symbol;
                else
                    black_symbol_occupation_map[coord_str] = symbol;


        };

        

        /*

        Move dependency tree

                           active_move  ->  changes active game parameters like move count
                                |           move is denoted, denotes en-passant possibility. Removes castling rights.
                                |           Refreshes the board position, by parsing new targets and moves.
                                v
        move_is_legal  -->  legal_move  ->  checks if the move is is legal via next dependency
        [simulation]            |           if so, will allow the ingorant move.
                ^               |           
                |               v
                |------  ignorant_move  ->  removes a piece at origin and places it at target,
                |                           regardless of open checks this may leave.
                |                           Also appendix moves like rook at castling or en-passant
                |                           caputures are recognized, played and denoted.
                |                           The saving of the last move allows to undo the last move only.
                |                           This is for pure simulation purposes not for depth analysis,
                |                           as this will be handled later by the engine.
                |
        undo_ignorant_move -> Undos the last ignorant move played by the last move information.

        */

        void ignorant_move (string origin_coord_str, string target_coord_str, char origin_symbol, int color, bool verbose=1) {

            /* Moves a piece disregarding chess rules by a combination of remove and place methods. 
            Open checks are disregarded, however additional en-passant and castling moves are accounted. 
            Also the move information are saved in global last move and capture variables. */

            __call__(__func__);
            // determine the piece 
            int piece = pieces.from_symbol(origin_symbol);
            
            
            // if captures then apriori denote the enemy symbol at target coord
            if (square_is_occupied_by_enemy(color, target_coord_str)) {

                // make sure to remove the captured piece from grid
                // and corresponding occupation map 
                // remove_piece(target_coord_str, verbose);

                last_captured_symbol = get_symbol_from_coord(target_coord_str);
                last_captured_symbol_coord = target_coord_str;

                // check if rook was captured at origin square to remove castling rights
                if (castling_right_q_w && target_coord_str == "A1" && last_captured_symbol == 'R')
                    castling_right_q_w = 0;
                else if (castling_right_k_w && target_coord_str == "H1" && last_captured_symbol == 'R')
                    castling_right_k_w = 0;
                else if (castling_right_q_b && target_coord_str == "A8" && last_captured_symbol == 'r')
                    castling_right_q_b = 0;
                else if (castling_right_k_b && target_coord_str == "H8" && last_captured_symbol == 'r')
                    castling_right_k_b = 0;

            } else {

                last_captured_symbol = '_';
                last_captured_symbol_coord = "";

            }
            
            // remove the piece from origin
            remove_piece(origin_coord_str, verbose);
            
            // place the piece at new target
            // place_symbol(origin_symbol, target_coord_str);
            place_piece(piece, color, target_coord_str, verbose);
            
            /* ---- Appendix Moves ---- */
            // check for castling, if so do another move with the rook
            if (piece == pieces.King) {

                // castles - plays the appendix rook move depending on active color
                int r = pieces.Rook;
                if (origin_coord_str == "E1" && target_coord_str == "G1") {
                    remove_piece("H1", verbose);
                    place_piece(r, color, "F1", verbose);
                } else if (origin_coord_str == "E1" && target_coord_str == "C1") {
                    remove_piece("A1", verbose);
                    place_piece(r, color, "D1", verbose);
                } else if (origin_coord_str == "E8" && target_coord_str == "G8") {
                    remove_piece("H8", verbose);
                    place_piece(r, color, "F8", verbose);
                } else if (origin_coord_str == "E8" && target_coord_str == "C8") {
                    remove_piece("A8", verbose);
                    place_piece(r, color, "D8", verbose);
                }

                // denote King's position
                if (color == pieces.w) 
                    white_king_coord = target_coord_str;
                else
                    black_king_coord = target_coord_str;
                
            }
            
            // check if en-passant is captured to remove the captured pawn, as it's misplaced
            else if (piece == pieces.Pawn) {
                
                if (target_coord_str == en_passant_coord) {
                    
                    char target_file_str = en_passant_coord[0]; 
                
                    // depending on color 
                    if (color == pieces.w)
                        // black pawn captured on 5th rank
                        last_captured_symbol_coord = target_file_str + '5';
                    else if (color == pieces.b) 
                        // white pawn captured on 4th rank
                        last_captured_symbol_coord = target_file_str + '4';
                    
                    remove_piece(last_captured_symbol_coord, 1); // bug 
                
                }

            }

            // override global variables but not game variables 
            // (since the move can be executed outside of a game)
            last_move[0] = origin_coord_str;
            last_move[1] = target_coord_str;
            
        };

        bool legal_move (string origin_coord_str, string target_coord_str, bool verbose, char symbol, int color) {

            /* Makes an ignorant move (without chaning gaming parameters) 
            only if legal, and returns a boolean. */
            
            __call__(__func__);
            // abort if not legal
            if (!move_is_legal(origin_coord_str, target_coord_str, symbol, color)) {

                cout << "move " << origin_coord_str << " -> " << target_coord_str << " is not legal." << endl;
            
                // test for debugging
                show_board();
                show_pgn();
                return false;
            
            } 
            
            // now the main ignorant move is legal
            ignorant_move(origin_coord_str, target_coord_str, symbol, color, verbose);

            return true;
        };
        
        MoveInfo active_move (string origin_coord_str, string target_coord_str, bool verbose=0) {

            /* Active moves manipulate the board and game parameters. */
            
            __call__(__func__);

            // variables
            MoveInfo info;
            char symbol = get_symbol_from_coord(origin_coord_str);
            int color = get_color_from_symbol(symbol);
            
            // char captured_symbol;

            // save info to moveinfo object
            info.legal = 0;
            info.symbol = symbol;
            info.color = color;

            // apriori denote the symbol which is captured, including en-passant
            if (square_is_occupied_by_enemy(color, target_coord_str)) {
                info.capture_symbol = get_symbol_from_coord(target_coord_str); 
                info.capture_coord = target_coord_str; 
            } else if (target_coord_str == en_passant_coord) {
                string pawn_coord;
                if (color == pieces.w)
                    // black pawn captured on 5th rank
                    pawn_coord = target_coord_str[0] + '5';
                else if (color == pieces.b) 
                    // white pawn captured on 4th rank
                    pawn_coord = target_coord_str[0] + '4';
                info.capture_symbol = get_symbol_from_coord(pawn_coord);
                info.capture_coord = pawn_coord;
            }

            // execute move if legal
            if (legal_move(origin_coord_str, target_coord_str, verbose, symbol, color)) {
                
                int piece = pieces.from_symbol(symbol);

                // denote all necessary information to info object
                info.legal = 1;
                info.origin = origin_coord_str;
                info.target = target_coord_str;
                info.en_passant_coord = en_passant_coord;
                if (color == pieces.w) {
                    info.targets = white_targets;
                    info.moves = white_moves;
                } else {
                    info.targets = black_targets;
                    info.moves = black_moves;
                }

                // denote the castling rights if king or rook move (for undo)
                info.castling_right_k_b = castling_right_k_b;
                info.castling_right_q_b = castling_right_q_b;
                info.castling_right_k_w = castling_right_k_w;
                info.castling_right_q_w = castling_right_q_w;

                // denotte if colors are checked
                info.white_is_checked = white_is_checked;
                info.black_is_checked = black_is_checked;
                
                // remember if en-passant was enabled for this move
                if (en_passant_coord != "-")
                    en_passant_was_on = en_passant_coord;
                else 
                    en_passant_was_on = "-";  

                // denote if a new en-passant possibility arises from this move
                if (piece != pieces.Pawn)
                    en_passant_coord = '-';
                if (piece == pieces.Pawn) {
                    int rank_origin = stoi(get_square_from_coord(origin_coord_str)["rank"]);
                    map<string, string> target_square = get_square_from_coord(target_coord_str);
                    int target_file = stoi(target_square["file"]),
                        target_rank = stoi(target_square["rank"]);

                    // finally check if the pawn is moved from origin square for two ranks at once
                    if ((active_color == pieces.w && rank_origin == 1 && target_rank == 3) || 
                        (active_color == pieces.b && rank_origin == 6 && target_rank == 4)) {
                        
                        char symbol;

                        // check if a new en-passant possibility pops up for enemy
                        int sign = 1, 
                            neighbour_file,
                            en_passant_target_rank;
                        string neighbour_coord;
                        
                        // iterate through left and right neighbouring squares
                        for (int x = 0; x < 2; x++) {
                            
                            sign *= -1;
                            neighbour_file = target_file+sign;
                            
                            if (inside_bounds(target_rank, neighbour_file)) {
                                
                                neighbour_coord = get_coord_from_file_and_rank(neighbour_file, target_rank);
                                symbol = get_symbol_from_coord(neighbour_coord);
                                
                                if (square_is_occupied_by_enemy(active_color, neighbour_coord) && pieces.from_symbol(symbol) == pieces.Pawn) {
                                    
                                    // find the correct en-passant rank based on color 
                                    // (rank is 1 behind the played pawn i.e. where the en-passant target will be captured)
                                    string en_passant_target_rank_str;
                                    if (color == pieces.b)
                                        en_passant_target_rank_str = "6";
                                    else
                                        en_passant_target_rank_str = "3";

                                    // finally merge and override en passant coord
                                    en_passant_coord = target_square["file"] + en_passant_target_rank_str;

                                    break;
                                }
                            
                            }

                        }

                    } else

                        en_passant_coord = '-';

                } 
                
                // check if King moved to remove castling rights
                else if (piece == pieces.King) {

                    // looses castling rights when king is moved
                    if (color == pieces.w) 
                        castling_right_k_w = 0, castling_right_q_w = 0;
                    else if (color == pieces.b) 
                        castling_right_k_b = 0, castling_right_q_b = 0;

                }
                    
                // check if a rook moved to remove castling right
                else if (piece == pieces.Rook) {
                    if (origin_coord_str == "H1")
                        castling_right_k_w = 0;
                    else if (origin_coord_str == "A1")
                        castling_right_q_w = 0;
                    else if (origin_coord_str == "A8")
                        castling_right_q_b = 0;
                    else if (origin_coord_str == "H8")
                        castling_right_k_b = 0;
                }

                // denote the half-move and append to history
                string move_notation = move_to_pgn(origin_coord_str, target_coord_str, symbol, color, info.capture_symbol);
                if (color == pieces.w)
                    pgn_history[move_count] = {};
                pgn_history[move_count].push_back(move_notation);
                // cout << "move notation: " << pgn_history[move_count][0] << endl;

                // refresh the board position
                refresh_position();

            }

            return info;
        }   

        void undo_ignorant_move () {

            /* Undos the last ignorant move. */
            // skip if there is no previous move defined

            __call__(__func__);

            if (last_move[0] == "" || last_move[1] == "") {
                return;
            }

            // undo the first main move
            string  target = last_move[0],
                    origin = last_move[1];
            char symbol = get_symbol_from_coord(origin);
            int piece = pieces.from_symbol(symbol);
            
            // use remove and place instead of ignorant move to keep the capture information persistent
            remove_piece(origin, 0);
            place_symbol(symbol, target);

            // check for appendix rook when castles and place it back to origin
            if (piece == pieces.King) {

                // undo castling
                int color = get_color_from_symbol(symbol);
                if (color == pieces.w)
                    if (target == "E1" && origin == "G1") {
                        remove_piece("F1", 0);
                        place_piece(pieces.Rook, color, "H1", 0);
                    } else if (target == "E1" && target == "C1") {
                        remove_piece("D1", 0);
                        place_piece(pieces.Rook, color, "A1", 0);
                    } 
                else if (color == pieces.b)
                    if (target == "E8" && target == "G8") {
                        remove_piece("H8", 0);
                        place_piece(pieces.Rook, color, "F8", 0);
                    } else if (target == "E8" && target == "C8") {
                        remove_piece("A8", 0);
                        place_piece(pieces.Rook, color, "D8", 0);
                    }

                // revert to old King's position
                if (color == pieces.w) 
                    white_king_coord = target;
                else
                    black_king_coord = target;

            } 
            
            // en-passant rank is corrected already in ignorant move.

            // if a piece was captured place it where it was
            if (last_captured_symbol != '_')
                place_symbol(last_captured_symbol, last_captured_symbol_coord);

            // remove the last move & capture information
            last_move[0] = "";
            last_move[1] = "";
            last_captured_symbol = '_';
            last_captured_symbol_coord = "";

        }

        void undo_active_move (MoveInfo info) {

            /* Unmakes the last active move. works on single depth only. */
            
            __call__(__func__);

            // draw the last move securely from persitent object
            const string    origin = info.target,
                            target = info.origin;
            
            const char symbol = info.symbol;
            const int piece = pieces.from_symbol(symbol);
            
            // switch back active color
            active_color = info.color;

            // reset castling rights
            castling_right_k_w = info.castling_right_k_w;
            castling_right_k_b = info.castling_right_k_b;
            castling_right_q_w = info.castling_right_q_w;
            castling_right_q_b = info.castling_right_q_b;

            // checks
            white_is_checked = info.white_is_checked;
            black_is_checked = info.black_is_checked;
            
            // bring back moves and targets for origin position
            if (active_color == pieces.w) {
                white_targets = info.targets;
                white_moves = info.moves;
            } else {
                black_targets = info.targets;
                black_moves = info.moves;
            }
            
            // note: the move history vector 'pgn_history[move_count]' is never empty, but contains at least 1 element
            if (!pgn_history.count(move_count)) {
                move_count--;
                pgn_history[move_count].pop_back();
            } else if (pgn_history[move_count].size() == 1)
                pgn_history.erase(move_count);
            else
                pgn_history[move_count].pop_back();
            
            // check for appendix rook when castles and place it back to origin
            if (piece == pieces.King) {

                // undo castling
                if (active_color == pieces.w)
                    if (target == "E1" && origin == "G1") {
                        remove_piece("F1", 0);
                        place_piece(pieces.Rook, active_color, "H1", 0);
                    } else if (target == "E1" && target == "C1") {
                        remove_piece("D1", 0);
                        place_piece(pieces.Rook, active_color, "A1", 0);
                    } 
                else if (active_color == pieces.b)
                    if (target == "E8" && target == "G8") {
                        remove_piece("H8", 0);
                        place_piece(pieces.Rook, active_color, "F8", 0);
                    } else if (target == "E8" && target == "C8") {
                        remove_piece("A8", 0);
                        place_piece(pieces.Rook, active_color, "D8", 0);
                    }

            } 

            // revert main move
            remove_piece(info.target, 0);
            place_symbol(info.symbol, info.origin);

            // revert the captured piece
            if (info.capture_coord != "") 
                place_symbol(info.capture_symbol, info.capture_coord);
            
            
            
        };

        map<string, vector<string>> get_possible_moves_for_active_color () {
            
            /* Returns a map of active color piece coordinates and the corresponding legal moves. */

            __call__(__func__);

            if (active_color == pieces.w)
                return white_moves;
            return black_moves;
        
        }

        void refresh_position () {

            /* Main parsing mechanism for position and map computation. 
            Should be called after every board alternation e.g. at the end of an active move. 
            The method works color-wise for efficiency reasons, and will gather
            all targets, moves and symbol mappings, for global access. */
            
            __call__(__func__);

            // flip the active color and raise move count
            if (active_color == pieces.b) {
                move_count++;
                active_color = pieces.w;
            } else {
                active_color = pieces.b;
            }

            // keep the recent moves and targets persistent
            // update target map and move object depending on color
            if (active_color == pieces.w) {

                // update targets for recent color after it played a move
                // targets are sufficient for determine possible checks for the next half move.
                update_reachable_target_map(pieces.b); // works but inefficient since all pieces have to be iterated
                // update_moves_from_targets(update_reachable_target_map(pieces.b), pieces.b);
                
                white_is_checked = is_checked(pieces.w);
                
                // update moves for new active color
                update_moves_from_targets(update_reachable_target_map(pieces.w), pieces.w);

                // if no moves are left it's a mate
                if (white_is_checked && white_moves.empty()) {
                    cout << pieces.color_string(active_color) << " got checkmated!" << endl;
                }

            } else {

                update_reachable_target_map(pieces.w);
                // update_moves_from_targets(update_reachable_target_map(pieces.w), pieces.w);

                black_is_checked = is_checked(pieces.b);
                
                update_moves_from_targets(update_reachable_target_map(pieces.b), pieces.b);

                // if no moves are left it's a mate
                if (black_is_checked && black_moves.empty()) {
                    cout << pieces.color_string(active_color) << " got checkmated!" << endl;
                }

            }

            // map all symbols to their current coordinate
            // update_symbol_map();
            

        }
            
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
                    etc.                                            */
            
            
            int rank, file;
            string coord;
            
            for (int i = 0; i < 64; i++) {
                
                // track rank and file
                rank = (int)( i * 0.125 + 1 );
                file = i % 8;

                // construct the coordinate by concatenating rank number and fileumn letter
                coord = letter_coordinates[file] + to_string(rank);

                // fill the info to the map at id "i"
                grid[i]["id"] = to_string(i);
                grid[i]["coordinate"] = coord;
                grid[i]["rank"] = to_string(rank-1);
                grid[i]["file"] = to_string(file);  
                grid[i]["symbol"] = "_";
                grid[i]["is_light_square"] = (file + rank) % 2 != 0; 
            }

        };
        /* ---- low-level board methods ---- */
        
        // bool can_castle_king_side (int color) {
            
        //     /* Checks if a color can castle king side. */

        //     __call__(__func__);
        //     if (!is_checked(color)) {
        //         if (color == pieces.w && castling_right_k_w) {
        //             if (!square_is_occupied("F1") && 
        //                 !square_is_occupied("G1") && 
        //                 !square_is_attacked("F1", pieces.b) && 
        //                 !square_is_attacked("G1", pieces.b))
        //                 return 1;
                    
        //         } else if (color == pieces.b && castling_right_k_b) {
        //             if (!square_is_occupied("F8") && 
        //                 !square_is_occupied("G8") && 
        //                 !square_is_attacked("F8", pieces.w) && 
        //                 !square_is_attacked("G8", pieces.w))
        //                 return 1;
        //         }
        //     }

        //     return 0;

        // };

        // bool can_castle_queen_side (int color) {

        //     /* Checks if a color can castle queen side. */

        //     __call__(__func__);
        //     if (!is_checked(color)) {

        //         if (color == pieces.w && castling_right_q_w) {

        //             if (!square_is_occupied("C1") && 
        //                 !square_is_occupied("D1") && 
        //                 !square_is_attacked("C1", pieces.b) && 
        //                 !square_is_attacked("D1", pieces.b))
        //                 return 1;
                    
        //         } else if (color == pieces.b && castling_right_q_b) {

        //             if (!square_is_occupied("C8") && 
        //                 !square_is_occupied("D8") && 
        //                 !square_is_attacked("C8", pieces.w) && 
        //                 !square_is_attacked("D8", pieces.w))
        //                 return 1;

        //         }
        //     }

        //     return 0;

        // };

        // 
        // bool move_is_legal (string origin_coord_str, string target_coord_str, char symbol, int origin_color) {
            
        //     /* Checks if a move is legal by general chess rules.
        //     The playable moves from origin are drawn from the targets map. */
            
        //     __call__(__func__);

        //     vector<string> playable_moves;
        //     int piece = pieces.from_symbol(symbol);

        //     // check if active color is respected
        //     if (origin_color == 0) {
        //         return false;
        //     } else if (origin_color != active_color) {
        //         cout << "it is " << pieces.color_string(active_color) << "'s turn!" << endl;
        //         return false;
        //     }
           
        //     // first check if move is a castling move and if it is valid, otherwise continue
        //     if (piece == pieces.King) {
        //         if (origin_color == pieces.w && origin_coord_str == "E1" && target_coord_str == "G1") {
        //             if (can_castle_king_side(origin_color))
        //                 return true;
        //             else
        //                 return false;
        //         }
        //         if (origin_color == pieces.b && origin_coord_str == "E8" && target_coord_str == "G8") {
        //             if (can_castle_king_side(origin_color))
        //                 return true;
        //             else
        //                 return false;
        //         }
        //         if (origin_color == pieces.w && origin_coord_str == "E1" && target_coord_str == "C1") {
        //             if (can_castle_queen_side(origin_color))
        //                 return true;
        //             else
        //                 return false;
        //         } 
        //         if (origin_color == pieces.b && origin_coord_str == "E8" && target_coord_str == "C8") {
        //             if (can_castle_queen_side(origin_color))
        //                 return true;
        //             else
        //                 return false;
        //         } 
        //     }
            
        //     // en-passant
        //     // was checked already in reachable_targets object, it only needs to be checked if the en-passant
        //     // move leaves an open check which will be done by default in the following.

        //     // make sure there is no check after move (possible bottleneck)
        //     // not sure if this is actually needed since the moves object contains the moves which leave no open check.
        //     // if (move_leaves_open_check(origin_coord_str, target_coord_str))
        //     //     return false;

        //     // insted try to find the corresponding move in pre-computed move object
        //     playable_moves = black_moves[origin_coord_str];
        //     if (origin_color == pieces.w)
        //         playable_moves = white_moves[origin_coord_str];
            
        //     for (int i = 0; i < playable_moves.size(); i++)
        //         if (target_coord_str == playable_moves[i])
        //             return 1;
            
            
        //     // if (contains_string(playable_moves, target_coord_str))
        //     //     return true;
        //     return false;
            
        // };

        // bool move_leaves_open_check (string origin_coord_str, string target_coord_str) {
            
        //     /* Checks by quick simulation if a move leaves an open check, otherwise move is not legal. */

        //     __call__(__func__);

        //     char origin_symbol = get_symbol_from_coord(origin_coord_str);
        //     int target_color,
        //         origin_color = get_color_from_symbol(origin_symbol);
        //     if (origin_color == pieces.w)
        //         target_color = pieces.b;
        //     else
        //         target_color = pieces.w;
            
        //     // simulate the move
        //     bool result;
        //     ignorant_move(origin_coord_str, target_coord_str, origin_symbol, origin_color, 0);
        //     if (is_checked(origin_color))
        //         result = true;
        //     else
        //         result = false;
            
        //     // revert position, the undo function will take care about castling and en-passant appendix moves
        //     // to do this quickly it uses the last move and capture information.
        //     undo_ignorant_move();
              
        //     return result;
            
        // };

        
        // vector<string> scoping_enemy_squares (string coord_str) {
            
        //     /* A function which returns all enemy squares whose direction points to a demanded square. 
        //     Regardless of wether a piece is blocking it. */

        //     __call__(__func__);

        //     vector<string> out = {};
        //     map <string, string> square = get_square_from_coord(coord_str);
        //     char symbol = square["symbol"][0];
            
        //     int color = get_color_from_symbol(symbol);

        //     string target_coord;
        //     char target_symbol;
        //     int piece,
        //         rank = stoi(square["rank"]),
        //         file = stoi(square["file"]); 

        //     // check diagonals for enemy queen or bishop 
        //     for (int i = -7; i < 8; i++) {
        //         if (inside_bounds(rank+i, file+i)) {
        //             target_coord = get_coord_from_file_and_rank(rank+i, file+i);
        //             target_symbol = get_symbol_from_coord(target_coord);
        //             piece = pieces.from_symbol(target_symbol);
        //             if (square_is_occupied_by_enemy(color, target_coord) && ( piece == pieces.Queen || piece == pieces.Bishop) ) {
        //                 out.push_back(target_coord);
        //             }
        //         }
        //         if (inside_bounds(rank-i, file+i)) {
        //             target_coord = get_coord_from_file_and_rank(rank-i, file+i);
        //             target_symbol = get_symbol_from_coord(target_coord);
        //             piece = pieces.from_symbol(target_symbol);
        //             if (square_is_occupied_by_enemy(color, target_coord) && ( piece == pieces.Queen || piece == pieces.Bishop) ) {
        //                 out.push_back(target_coord);
        //             }
        //         }
        //     }

        //     // check straight lines for enemy rook or queen 
        //     for (int i = -7; i < 8; i++) {
        //         if (inside_bounds(rank+i, file)) {
        //             target_coord = get_coord_from_file_and_rank(rank+i, file+i);
        //             target_symbol = get_symbol_from_coord(target_coord);
        //             piece = pieces.from_symbol(target_symbol);
        //             if (square_is_occupied_by_enemy(color, target_coord) && ( piece == pieces.Queen || piece == pieces.Bishop) ) {
        //                 out.push_back(target_coord);
        //             }
        //         }
        //         if (inside_bounds(rank, file+i)) {
        //             target_coord = get_coord_from_file_and_rank(rank-i, file+i);
        //             target_symbol = get_symbol_from_coord(target_coord);
        //             piece = pieces.from_symbol(target_symbol);
        //             if (square_is_occupied_by_enemy(color, target_coord) && ( piece == pieces.Queen || piece == pieces.Rook) ) {
        //                 out.push_back(target_coord);
        //             }
        //         }
        //     }
            
        //     return out;

        // };
    
        // vector<string> square_is_attacked_by_coords (string coord_str, map<string, vector<string>> enemy_targets) {

        //     /* Returns all enemy (attacker) coordinates which are hitting the square. 
        //     The coordinates are determined from the provided enemy target map.*/

        //     __call__(__func__);

        //     string attacker_coord;
        //     vector<string> attacking_coords;
        //     vector<string> out = {};

        //     for (auto const& x : enemy_targets) {

        //         // coordinate of attacker
        //         attacker_coord = x.first;

        //         // coordinates being attacked
        //         attacking_coords = x.second;

        //         for (int i = 0; i < attacking_coords.size(); i++) {
        //             if (attacking_coords[i] == coord_str) {
        //                 out.push_back(attacker_coord);
        //                 break;
        //             }
        //         }
                
        //     }

        //     return out;
            
        // };

        // bool square_is_attacked (string coord_str, int attacker_color) {

        //     /* A faster version of square_is_attacked_by_coords, which only
        //     returns truth value at first found instance i.e. if the enemy's
        //     targets yield the coord_str. */
            
        //     __call__(__func__);

        //     // string attacker_coord;
        //     // vector<string> attacking_coords;
        //     map<string, vector<string>> attacker_targets;

        //     // select attacker targets from enemy color
        //     if (attacker_color == pieces.w) 
        //         attacker_targets = white_targets;
        //     else
        //         attacker_targets = black_targets;

        //     for (auto const& x : attacker_targets) {

        //         // origin coordinate of attacker piece
        //         // attacker_coord = x.first;
        //         // attacking_coords = x.second;

        //         for (int i = 0; i < x.second.size(); i++)
        //             if (x.second[i] == coord_str)
        //                 return true;

        //         // as soon as the attacking coordinates contain the coord_str
        //         // the provided coordinate is considered attacked
        //         // if (contains_string(x.second, coord_str))
        //         //     return true;

        //     }

        //     return false;

        // };

        // bool is_checked (int color) {

        //     /* Returns true if the color is in check. */
            
        //     __call__(__func__);

        //     string coord;
        //     int attacker_color;

        //     // iterate through board to find the king's position
        //     if (color == pieces.w) {
        //         coord = white_king_coord;   
        //         attacker_color = pieces.b;
        //     } else {
        //         coord = black_king_coord;
        //         attacker_color = pieces.w;
        //     }

        //     // check if king's coordinate is being attacked
        //     return square_is_attacked(coord, attacker_color);

        // }

        // bool is_mate (int color) {

        //     /* Returns truth value based on if a color is being mated in current position. */

        //     __call__(__func__);

        //     if (color == pieces.w)
        //         return white_is_checked && white_moves.empty();
        //     return black_is_checked && black_moves.empty();
        
        // }

        // bool square_is_in_line (string coord1, string coord2, string probe_coord) {

        //     /* Checks quickly if a probing coordinate lies within the aligned line (vert.,hor.,diag.) 
        //     spanned by the first two coordinates. If the first two coordinates are not mutually aligned 
        //     or they are not aligned with the probing coordinate, the return will be 0. */

        //     __call__(__func__);

        //     int r1 = stoi(get_square_from_coord(coord1)["rank"]),
        //         r2 = stoi(get_square_from_coord(coord2)["rank"]),
        //         r3 = stoi(get_square_from_coord(probe_coord)["rank"]),
        //         f1 = stoi(get_square_from_coord(coord1)["file"]),
        //         f2 = stoi(get_square_from_coord(coord2)["file"]),
        //         f3 = stoi(get_square_from_coord(probe_coord)["file"]);
            
        //     // all pieces are aligned horizontally
        //     if (r1 == r2 && r3 == r1) {
        //         if (f1 > f2 && f1 > f3 && f3 > f2 || f2 > f1 && f3 > f1 && f2 > f3)
        //             return 1;
        //     } 
        //     // all pieces are aligned vertically
        //     else if (f1 == f2 && f3 == f1) {
        //         if ( (r1 > r2 && r1 > r3 && r3 > r2) || (r2 > r1 && r2 > r3 && r3 > r1)) 
        //             return 1;
        //     } 
        //     // all pieces are aligned diagonally
        //     else if (abs(f1-f2) == abs(r1-r2) && abs(r1-r3) == abs(f1-f3) && (r1-r2)/abs(r1-r2) == abs(r1-r3)/abs(r1-r3) && (f1-f2)/abs(f1-f2) == abs(f1-f3)/abs(f1-f3)) {
        //         if ( !(f1 > f2 && f1 > f3 && f3 > f2) || !(f2 > f1 && f2 > f3 && f3 > f1) ) 
        //             return 0;
        //         if ((r1 > r2 && r1 > r3 && r3 > r2) || (r2 > r1 && r2 > r3 && r3 > r1) ) 
        //             return 1;
        //     }

        //     return 0;
        // };

        

        // bool square_is_occupied_by_enemy (int friendly_color, string coord_str) {

        //     /* Returns boolean-like integer. The return is 1 if the square is occupied by enemy piece otherwise 0 */

        //     __call__(__func__);

        //     char target_symbol = get_symbol_from_coord(coord_str);
        //     int target_color = get_color_from_symbol(target_symbol); 

        //     return target_color != friendly_color && target_color != 0;

        // }

        // bool square_is_occupied_by_friendly_piece (int friendly_color, string coord_str) {
            
        //     /* Returns bool dep. on wether a friendly piece persists on this coordinate. */

        //     __call__(__func__);

        //     char target_symbol = get_symbol_from_coord(coord_str);
        //     int target_color = get_color_from_symbol(target_symbol);

        //     return target_color == friendly_color;

        // }


        
        /*  update moves and target methods ---- */

        
        // map<string, vector<string>> update_reachable_target_map (int color) {

        //     /* Updates reachable target map for provided color and 
        //     overrides the persistent global variable white_targets/black. 
        //     The overriden map will be returned at the end. */

        //     __call__(__func__);

        //     // string coord;
        //     map<string, vector<string>> m;
        //     map<string, char> occupation_map;

        //     // pick occupation map for provided color
        //     if (color == pieces.w)
        //         occupation_map = white_symbol_occupation_map;
        //     else
        //         occupation_map = black_symbol_occupation_map;
            
        //     // iterate through field (deprecated since too inefficient)
        //     // for (int i = 0; i < 64; i++) {
        //     //     coord = get_coord_from_id(i);
        //     //     if (square_is_occupied_by_friendly_piece(color, coord)) {
        //     //         // override/create entry for coord with vector of all reachable targets from that square
        //     //         m[coord] = reachable_target_coords(coord);
        //     //     } 
        //     // }

        //     // iterate only the valid piece coordinates from occupation map
        //     // this is better than the dedicated approach by screening the whole 64-field grid
        //     for (auto const& x : occupation_map) {

        //         // extract friendly piece coord from occupation map
        //         // coord = x.first; // coord
                
        //         // add the reachable targets map
        //         m[x.first] = reachable_target_coords(x.first, x.second, color);

        //         // cout << "target coord: " << x.first << " " << x.second << endl;

        //     }


            
        //     // override & return
        //     if (color == pieces.w) {
        //         white_targets = m;
        //     } else {
        //         black_targets = m;
        //     }

        //     return m;
            
        // }

        // map<string, vector<string>> update_moves_from_targets (map<string, vector<string>> target_map, int color) {
            
        //     /* Filters the legal moves from target map and overrides the moves object. 
        //     Only moves which leave no open checks. */
            
        //     __call__(__func__);

        //     string origin_coord;
        //     vector<string> targets;
        //     map<string, vector<string>> moves;

        //     for (auto const& x : target_map) {

        //         origin_coord = x.first;
        //         targets = x.second;
                
        //         for (int i = 0; i < targets.size(); i++) {

        //             if (!move_leaves_open_check(origin_coord, targets[i])) {

        //                 // only add the piece coord key to the moves 
        //                 // object if there are moves at all
        //                 if (!moves.count(origin_coord))
        //                     moves[origin_coord] = {};
                        
        //                 moves[origin_coord].push_back(targets[i]);
        //             }

        //         }
                
        //     }
            
        //     // override
        //     if (color == pieces.w) {
        //         white_moves = moves;
        //         return white_moves;
        //     } else {
        //         black_moves = moves;
        //         return black_moves;
        //     }
            
        // }



        // ---- new method set ----

        int active_color = pieces.w;

        // prepare letters for the board files
        const char letter_coordinates[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
        const char number_coordinates[8] = {'1', '2', '3', '4', '5', '6', '7', '8'};
        const string starting_position_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

        // also define a map symbol -> coord(symbol)
        // map<char, vector<string>> coord_symbol_map;
        map<string, char> white_symbol_occupation_map; // maps coord to symbol
        map<string, char> black_symbol_occupation_map;

        // Load the pieces
        Piece pieces;
        map<int, vector<string>> pgn_history;
        int move_count = 1;

        // castling rights
        bool all_castling_rights_lost = 0;
        bool castling_right_k_w = 1;
        bool castling_right_k_b = 1;
        bool castling_right_q_w = 1;
        bool castling_right_q_b = 1;

        // king scope
        bool white_is_checked = false;              // remember check after every active move
        bool black_is_checked = false;
        vector<string> check_coords = {};           // attacker coords, symbols can be derived from occ. map
        vector<string>  black_scopers,              // scopers target the king with or without a pinned piece in-between
                        white_scopers;
        map<string, vector<string>> white_pins,     // pin to spaces mapping for pinned pieces
                                    black_pins;
        map<string, vector<string>> white_spaces,   // spaces between attacker and king when in scoped check (no night nor pawn)
                                    black_spaces; 

        // track king coordinates
        string  white_king_coord = "E1", 
                black_king_coord = "E8";

         /* Denote everything for the next move i.e. from current position 
            map ->  attacker_coord: [attacking_coord_1, 2, ...]
                    attacker_coord: [attacking_coord_1, 2, ...]
                    ...
        */
        map<string, vector<string>> white_targets,
                                    black_targets,
                                    white_moves,
                                    black_moves;

        /* define relative decrementation coordinates, which persist
        static declaration makes sure all board instances use the same value. */ 
        const vector<vector<int>> n_offset {{1,2}, {2,1}, {-1,2}, {-2,1}, {-1,-2}, {-2,-1}, {1,-2}, {2,-1}}; // knight relative coordinates
        const vector<vector<int>> d_offset {{1,1}, {1,-1}, {-1,-1}, {-1,1}};
        const vector<vector<int>> v_offset {{1,0}, {-1,0}};
        const vector<vector<int>> h_offset {{0,1}, {0,-1}};  

        // coordinate and symbol methods
        int get_color_from_symbol (char symbol) {

            /* Returns the piece color int from symbol char e.g. k -> 16, Q -> 8
            where 16 is black and 8 white. If the symbol is None '_' the return will be 0. */

            __call__(__func__);
            if (pieces.is_white(symbol))
                return 8;
            else if (symbol == '_')
                return 0;
            else
                return 16; 

        };
        
        string get_coord_from_file_and_rank (int file, int rank) {
            
            /* Returns the coordinate string for provided rank and file integers.
            e.g. (0,0) -> A1 or (0,1) -> A2 etc.*/

            // return get_coord_from_id(file + 8 * rank);
            return letter_coordinates[file] + to_string(rank+1); // faster method

        };

        vector<int> get_file_and_rank_from_coord (string coord_str) {
            /* Returns a 2d vector with file and rank (between 0 and 7). */
            vector<int> out;
            for (int i = 0; i < 7; i++) {
                if (letter_coordinates[i] == coord_str[0]) {
                    out.push_back(0);
                    break;
                }
            }
            out.push_back(coord_str[1] - '0');
            return out;
        };

        map<string, vector<string>> get_moves (int color) {
            /* Returns the target map for provided color. */
            if (color == pieces.w)
                return white_moves;
            else
                return black_moves;
        }

        map<string, char> get_occupation_map (int color) {

            /* Returns the occupation map for provided color. */

            if (color == pieces.w)
                return white_symbol_occupation_map;
            else
                return black_symbol_occupation_map;

        }

        map<string, vector<string>> get_targets (int color) {
            /* Returns the target map for provided color. */
            if (color == pieces.w)
                return white_targets;
            else
                return black_targets;
        }

        char get_symbol_from_coord (string coord_str) {
            /* A fast method integration to find symbol from coordinate via occupation lookup. */
            if (white_symbol_occupation_map.count(coord_str))
                return white_symbol_occupation_map[coord_str];
            else if (black_symbol_occupation_map.count(coord_str))
                return black_symbol_occupation_map[coord_str];
            else
                return pieces.None;
        };
        
        // board operations
        void flip_active_color () {
            if (active_color == pieces.w)
                active_color = pieces.b;
            else
                active_color = pieces.w;
        }

        void clear () {

            /* Removes all pieces from the board. */

            for (int i = 0; i < 64; i++)
                grid[i]["symbol"] = '_';
            
        };

        void load_position_from_fen (string fen, bool verbose=1) {

            /* A fen parsing implementation which generates a position from compact string.
            The reading starts from upper left corner i.e. rank is decremented while the files
            are iterated from left to right. */

            int piece, 
                color, 
                file = 1, 
                rank = 8, 
                pieces_completely_parsed = 0,
                active_color_parsed = 0,
                castling_parsed = 0,
                en_passant_parsed = 0,
                half_clock_parsed = 0,
                delimiter_index,
                move_count_parsed = 0;
            char _char;
            char delimiter = '/';
            string coord;
            string moves = "";
            string castling_options = "KkQq-";

            
            // first exchange all slashes for unique symbols to avoid escapes
            while (delimiter_index != string::npos) {
                delimiter_index = fen.find(delimiter);
                fen[delimiter_index] = '&';
            }
            
            // delete global variables 
            en_passant_coord = "";
            moves = "";
            castling_right_k_w = 0;
            castling_right_k_b = 0;
            castling_right_q_w = 0;
            castling_right_q_b = 0;
            
            // parse ...
            for (int i = 0; i < fen.size(); i++) {
                
                _char = fen[i];

                // parse piece locations
                if (!pieces_completely_parsed) {

                    // determine piece and color from symbol char
                    piece = pieces.from_symbol(_char);
                    
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
                    
                    // otherwise _char is definitely a piece symbol
                    if (pieces.is_white(_char)) {
                        color = 8;
                    } else {
                        color = 16;
                    }
                    
                    // determine the id from current rank and file pointer
                    coord = get_coord_from_file_and_rank(file, rank);
                    // coord = get_coord_from_file_and_rank(file, rank);

                    // place the piece
                    place_symbol_at_coord(_char, coord);

                    // override king position
                    if (_char == 'k')
                        black_king_coord = coord;
                    else if (_char == 'K')
                        white_king_coord = coord;

                    // increment file
                    file++;
                
                // check for active color & override
                } else if (pieces_completely_parsed && !active_color_parsed && (_char == 'b' || _char == 'w')) {
                    if (_char == 'w')
                        active_color = pieces.w;
                    else
                        active_color = pieces.b;
                    active_color_parsed = 1;
                    i++;

                // check for castling rights 
                } else if (pieces_completely_parsed && active_color_parsed && !castling_parsed ) {

                    if (_char == ' ') {
                        castling_parsed = 1;
                    } else if (_char == '-') {
                        castling_parsed = 1;
                        i++;
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
                        move_count = stoi(moves);
                        if (verbose)
                            print("Successfully loaded position from FEN.", "Board");
                        // finished   
                    }
                } 

            };

            // initialize all possible moves for white
            update_targets_and_moves(active_color);
            // update_moves_from_targets(update_reachable_target_map(active_color), active_color);
            
        };
        
        void load_starting_position (bool verbose=1) {
            
            /* Loads all pieces to the board grid by using their 
            symbol values. Start position fen is loaded via fen parser.*/

            __call__(__func__);

            if (verbose)
                cout << "load starting position ..." << endl;

            // load position from FEN code    
            load_position_from_fen(starting_position_fen, verbose);

            if (verbose)
                cout << "successfully loaded starting position." << endl;

        };

        string move_to_pgn (string origin_coord_str, string target_coord_str, char origin_symbol, int origin_color, char target_symbol) {
            
            /*
            Converts a move to portable game notation string, depending on board postion etc.
            This function should be called within the active move function but before the ignorant move within is played,
            as this would alter the board position.

            target_symbol       Provided for enemy target symbols, otherwise '_'.
                                Empty target is provided by taget_symbol='_', 
                                otherwise the provided piece will be considered captured. 
            */

            // map<string, string> origin_square = get_square_from_coord(origin_coord_str);
            // char origin_symbol = origin_square["symbol"][0];
            char symbol_cap = toupper(origin_symbol);
            // int color = get_color_from_symbol(origin_symbol);
            vector<int> fr_origin = get_file_and_rank_from_coord(origin_coord_str);
            int origin_piece = pieces.from_symbol(origin_symbol);
            char origin_file_str = origin_coord_str[0];
            string attacker_coord;
            string formatted = target_coord_str; // output

            /* ---- castling ---- */
            // castling shortcut
            if ( origin_piece == pieces.King && ((origin_coord_str == "E1" && target_coord_str == "G1") || (origin_coord_str == "E8" && target_coord_str == "G8")))
                return "O-O"; // king-side castle
            else if (origin_piece == pieces.King && ((origin_coord_str == "E1" && target_coord_str == "C1") || (origin_coord_str == "E8" && target_coord_str == "C8")))
                return "O-O-O"; // queen-side castle

            // check if move will capture
            // if (square_is_occupied_by_enemy(color, target_coord_str))
            //     formatted = 'x' + formatted;
            // check if the target symbol is provided
            // in this case it will certainly be an enemy symbol
            if (target_symbol != '_') {

                // target is not empty - target_symbol is captured
                formatted = 'x' + formatted;

            }
            
            /* ---- pawn ---- */
            // check if pawn captured
            if (origin_piece == pieces.Pawn && formatted[0] == 'x')
                return lower_case(origin_file_str + formatted);
            // check if pawn is just pushed
            else if (origin_piece == pieces.Pawn && formatted[0] != 'x')
                return lower_case(formatted);

            /*  First check how many pieces attack the target coordinate.
                The aim is to determine if multiple pieces of same kind are attaking. */
            
            // get possible moves map
            map<string, vector<string>> m;
            if (origin_color == pieces.w)
                m = white_moves;
            else
                m = black_moves;

            // iterate through all other attackers of origin color, if another attacker
            // is the same piece as the origin piece, we need to distinguish either the file or rank
            int attacker_rank, attacker_file, attacker_piece;
            vector<int> fr_target; // file and rank buffer
            map<string, char> attacker_occupation_map;
            if (origin_color == pieces.w)
                attacker_occupation_map = get_occupation_map(pieces.b);
            else
                attacker_occupation_map = get_occupation_map(pieces.w);

            for (auto const& x : m) {
                // if attacker square is different from origin proceed
                if (x.first != origin_coord_str) {
                    
                    // if the other attacker square is attacking the target as well proceed
                    if (contains_string(x.second, target_coord_str)) {

                        attacker_coord = x.first;
                        fr_target = get_file_and_rank_from_coord(origin_coord_str);
                        attacker_file = fr_target[0], attacker_rank = fr_target[1];
                        attacker_piece = pieces.from_symbol(attacker_occupation_map[attacker_coord]);
                        
                        // if the attacking piece is no pawn and is one of two pieces of same kind which attack add the file
                        if (attacker_piece == origin_piece) {
                            
                            // if on same file denote the rank
                            if (attacker_file == fr_origin[0]) 
                                return symbol_cap + lower_case(origin_coord_str[1] + formatted);

                            // otherwise denote file
                            return symbol_cap + lower_case(origin_file_str + formatted);

                        }

                    }
                }
                
            }

            return symbol_cap + lower_case(formatted);

        };

        void place_symbol_at_coord (char symbol, string coord) {
            /* Ignorantly places a symbol at a specific coordinate by overriding
            the value in the occupation map for this coordinate. Note that
            the overrided value will be lost. 
            No tracking other tracking of game variables. */
            int color = get_color_from_symbol(symbol);
            if (color == pieces.w) {
                white_symbol_occupation_map[coord] = symbol;
            } else {
                black_symbol_occupation_map[coord] = symbol;
            }
        }

        void remove_symbol_at_coord (char symbol, string coord) {
            /* Ignorantly removes a symbol from corresponding occupation map. 
            No tracking other tracking of game variables.*/
            int color = get_color_from_symbol(symbol);
            if (color == pieces.w)
                white_symbol_occupation_map.erase(coord);
            else
                black_symbol_occupation_map.erase(coord);
        }

        snapshot move (string origin_coord_str, string target_coord_str, bool force=false) {

            /* Active move that tracks game variables like PGN history, king coordinate, en-passant etc.
            At every call the current state is recorded and returned for reversion to former state. 
            Will remove an empty object if the move is illegal. */

            // variables
            char symbol = get_symbol_from_coord(origin_coord_str);
            int color = get_color_from_symbol(symbol);
            map<string, char> occupation_map = get_occupation_map(color);
            snapshot snap;
            map<string, vector<string>> targets = get_targets(color);
            map<string, vector<string>> moves = get_targets(color); 
            char captured_symbol = get_symbol_from_coord(target_coord_str); // denote captured symbol for later notation

            // check if the move is legal by probing all illegal conditions, 
            // except when it's forced, then skip
            if (!force) {
                // wrong color is playing
                if (color != active_color) {
                    print("It's not " + pieces.color_string(color) + "'s turn.", "board");
                    return snap;
                }
                // origin coordinate does not 
                if (occupation_map.count(origin_coord_str) == 0) {
                    print(origin_coord_str + "holds no " + pieces.color_string(color) + " pieces.", "board");
                    return snap;
                }
                // target
                if (!contains_string(moves[origin_coord_str], target_coord_str)) {
                    print(pieces.color_string(color) + "'s move " + origin_coord_str + "->" + target_coord_str + " is not legal.", "board");
                    return snap;
                }
                if (color == get_color_from_symbol(captured_symbol)) {
                    print(target_coord_str + " is occupied by friendly piece.", "board");
                    return snap;
                }
            }

            // save info to snapshot object
            snap = record_snapshot();

            // take action on board
            remove_symbol_at_coord(symbol, origin_coord_str);
            place_symbol_at_coord(symbol, target_coord_str);

            // piece specific cases
            int piece = pieces.from_symbol(symbol);

            // check if pawn move leaves an en-passant possibility
            if (piece == pieces.Pawn) {

                // int rank_origin = stoi(get_square_from_coord(origin_coord_str)["rank"]);
                // map<string, string> target_square = get_square_from_coord(target_coord_str);
                // int target_file = stoi(target_square["file"]),
                //     target_rank = stoi(target_square["rank"]);
                int rank_origin = get_file_and_rank_from_coord(origin_coord_str)[1];
                vector<int> fr = get_file_and_rank_from_coord(target_coord_str);
                int target_file = fr[0], target_rank = fr[1];
                
                // finally check if the pawn is moved from origin square for two ranks at once
                if ((active_color == pieces.w && rank_origin == 1 && target_rank == 3) || 
                    (active_color == pieces.b && rank_origin == 6 && target_rank == 4)) {
                    
                    char symbol;

                    // check if a new en-passant possibility pops up for enemy
                    int sign = 1, 
                        neighbour_file,
                        en_passant_target_rank;
                    string neighbour_coord;
                    string en_passant_target_rank_str;
                    
                    // iterate through left and right neighbouring squares
                    for (int x = 0; x < 2; x++) {
                        
                        sign *= -1;
                        neighbour_file = target_file+sign;
                        
                        if (inside_bounds(target_rank, neighbour_file)) {
                            
                            neighbour_coord = get_coord_from_file_and_rank(neighbour_file, target_rank);
                            symbol = get_symbol_from_coord(neighbour_coord);
                            
                            if (square_is_occupied_by_enemy(active_color, neighbour_coord) && pieces.from_symbol(symbol) == pieces.Pawn) {
                                
                                // find the correct en-passant rank based on color 
                                // (rank is 1 behind the played pawn i.e. where the en-passant target will be captured)
                                if (color == pieces.b)
                                    en_passant_target_rank_str = "6";
                                else
                                    en_passant_target_rank_str = "3";

                                // finally merge and override the en-passant coord
                                en_passant_coord = letter_coordinates[target_file] + en_passant_target_rank_str;

                                break;

                            }
                        
                        }

                    }

                } else

                    en_passant_coord = '-';

            } 
            
            // check if King moved to remove castling rights
            else if (piece == pieces.King) {

                // looses castling rights when king is moved
                if (color == pieces.w) 
                    castling_right_k_w = 0, castling_right_q_w = 0;
                else if (color == pieces.b) 
                    castling_right_k_b = 0, castling_right_q_b = 0;

            }
                    
            // check if a rook moved to remove castling right
            else if (piece == pieces.Rook) {

                if (origin_coord_str == "H1")
                    castling_right_k_w = 0;
                else if (origin_coord_str == "A1")
                    castling_right_q_w = 0;
                else if (origin_coord_str == "A8")
                    castling_right_q_b = 0;
                else if (origin_coord_str == "H8")
                    castling_right_k_b = 0;

            }

            // denote the move and add to history
            string move_notation = move_to_pgn(origin_coord_str, target_coord_str, symbol, color, captured_symbol);
            if (color == pieces.w)
                pgn_history[move_count] = {};
            pgn_history[move_count].push_back(move_notation);

            // update board
            move_count++;
            flip_active_color();
            update_king_scopes(active_color);
            update_targets_and_moves(active_color);

        }

        snapshot record_snapshot () {
            /* Records and returns snapshot. */
            snapshot snap;
            snap.color = active_color;
            snap.move_count = move_count;
            snap.pgn_history = pgn_history;
            snap.en_passant_coord = en_passant_coord;
            snap.white_is_checked = white_is_checked;
            snap.black_is_checked = black_is_checked;
            snap.all_castling_rights_lost = all_castling_rights_lost;
            snap.castling_right_k_w = castling_right_k_w;
            snap.castling_right_q_w = castling_right_q_w;
            snap.castling_right_k_b = castling_right_k_b;
            snap.castling_right_q_b = castling_right_q_b;
            snap.white_king_coord = white_king_coord;
            snap.black_king_coord = black_king_coord;
            snap.white_occupation_map = white_symbol_occupation_map;
            snap.black_occupation_map = black_symbol_occupation_map;
            snap.white_targets = white_targets;
            snap.black_targets = black_targets;
            snap.white_moves = white_moves;
            snap.black_moves = black_moves;
            snap.white_pins = white_pins;
            snap.black_pins = black_pins;
            snap.white_spaces = white_spaces;
            snap.black_spaces = black_spaces;
            return snap;
        }


        // chess logic
        // constructor sequence
        Board(void) {

            // initialize grid
            // build_grid();
            
        };

        
        
        bool inside_bounds (int rank, int file) {

            /* Checks if the rank and file integer provided map to a valid chess square. */

            __call__(__func__);

            // simply check if the indices lie within the boundaries
            return rank >= 0 && rank < 8 && file >= 0 && file < 8;
            
        };

        vector<string> reachable_target_coords (string coord_str, char symbol, int color) {

            /* 
            Returns a string vector of reachable target coordinates. 
            If the coord_str is empty ('_') the output vector will be empty as well.
            */

            __call__(__func__);
            vector<string> out = {};
            // map <string, string> square = get_square_from_coord(coord_str);
            // char symbol = square["symbol"][0];
            
            // cut short in case the square is empty
            if (symbol == '_') {return out;}

            vector<vector<int>> decrement;
            // int color = get_color_from_symbol(symbol);
            int piece = pieces.from_symbol(symbol);

            // extract file and rank from origin coord
            string target_coord;
            vector<int> fr = get_file_and_rank_from_coord(coord_str);
            int file = fr[0], rank = fr[1];
            
            vector<char> screeningDirections;

            // pick the screening directions depending on piece
            if (piece == pieces.Pawn) 
                screeningDirections.push_back('p');
            else if (piece == pieces.Knight)
                screeningDirections.push_back('n');
            else if (piece == pieces.Bishop)
                screeningDirections.push_back('d');
            else if (piece == pieces.Rook)
                screeningDirections = {'v', 'h'};
            else if (piece == pieces.Queen || piece == pieces.King)
                screeningDirections = {'v', 'h', 'd'};
                
            // iterate through selected cases
            for (int i = 0; i < screeningDirections.size(); i++) {
                
                if (screeningDirections[i] == 'p') { // edge case for pawn

                    int sign=1, steps=1;
                    if (color == pieces.b) sign = -1;
                    if ((sign == 1 && rank == 1) || (sign == -1 && rank == 6)) steps = 2;

                    /* push forward */
                    // for loop is needed if steps=2 from origin
                    for (int step = 1; step <= steps; step++) {
                        // assign coord if valid
                        if (!inside_bounds(file, rank+step*sign)) 
                            break; 
                        target_coord = get_coord_from_file_and_rank(file, rank+step*sign);
                        if (square_is_occupied(target_coord))
                            break;
                        out.push_back(target_coord);
                    }

                    /* captures */
                    for (int i = 0; i < 2; i++) {
                        if (inside_bounds(file+pow(-1.,i), rank+sign)) {
                            
                            target_coord = get_coord_from_file_and_rank(file+pow(-1.,i), rank+sign);

                            // this coord should always be pushed to restrict enemy king 
                            // if (square_is_occupied_by_enemy(color, target_coord)) 
                            //     out.push_back(target_coord);

                            // this makes more sense, only do not denote the target if it's a friendly piece
                            if (!square_is_occupied_by_friendly_piece(color, target_coord))
                                out.push_back(target_coord);

                        }
                    }
                    
                    /* en-passant */
                    if (en_passant_coord != "-") {
                        // check if the selected pawn is next to en-passant coord
                        vector<int> fr = get_file_and_rank_from_coord(coord_str);
                        int ep_file = fr[0], ep_rank = fr[1];
                        // int ep_fil = stoi(ep_square["file"]);
                        if (rank == ep_rank-sign && (file+1 == ep_file || file-1 == ep_file))
                            out.push_back(en_passant_coord);
                    }
                    
                } else if (screeningDirections[i] == 'n') { // edge case for knight
                    int r, f;
                    for (int i = 0; i < 8; i++) {
                        r = rank + n_offset[i][0], 
                        f = file + n_offset[i][1];
                        if (inside_bounds(r, f)) {
                            target_coord = get_coord_from_file_and_rank(f, r);
                            if (!square_is_occupied(target_coord) || square_is_occupied_by_enemy(color, target_coord))
                                out.push_back(target_coord);
                        }
                    }
                } else if (screeningDirections[i] == 'd')   // load diagonal decrementation coords
                    decrement = d_offset;
                else if (screeningDirections[i] == 'v')   // load vertical decrementation coords
                    decrement = v_offset;
                else if (screeningDirections[i] == 'h')   // load horizontal decrementation coords
                    decrement = h_offset;

                // decrementation algorithm for v, h, d
                if (screeningDirections[i] == 'v' || screeningDirections[i] == 'h' || screeningDirections[i] == 'd') {
                    
                    int r, f, steps;
                    
                    if (piece == pieces.King)
                        steps = 2; // will screen only for 1 step
                    else
                        steps = 8; // 7 steps

                    // iterate direction
                    for (int i = 0; i < decrement.size(); i++) {

                        for (int step = 1; step < steps; step++) {

                            r = rank + step * decrement[i][0],
                            f = file + step * decrement[i][1];
                            
                            if (inside_bounds(r, f)) {
                            
                                target_coord = get_coord_from_file_and_rank(f,r);
                                
                                if (square_is_occupied_by_enemy(color, target_coord)) {
                                    
                                    /* for the king to take on target square it needs to be 
                                    decided if the target coord is protected */
                                    if (piece == pieces.King) {

                                        // determine attacker color
                                        int attacker_color;
                                        if (color == pieces.w)
                                            attacker_color = pieces.b;
                                        else
                                            attacker_color = pieces.w;

                                        // only consider the target square if its not protected
                                        if (!square_is_attacked(target_coord, attacker_color)) {
                                            out.push_back(target_coord);
                                        }

                                    } 

                                    // all other pieces can take the piece without regards
                                    else {
                                        out.push_back(target_coord);
                                    }

                                    break;

                                } else if (!square_is_occupied(target_coord)) 
                                    
                                    out.push_back(target_coord);
                                    
                                else

                                    // break when the quare is occupied by friendly piece
                                    break;

                            }
                            
                        }
                    }
                }
                
            }
            
            // check for castling opportunity in case of king
            if (piece == pieces.King) {
                if (can_castle_king_side(color)) {
                    if (color == pieces.w) 
                        out.push_back("G1");
                    else
                        out.push_back("G8");
                }
                if (can_castle_queen_side(color)) {
                    if (color == pieces.w) 
                        out.push_back("C1");
                    else
                        out.push_back("C8");
                }
            }

            return out;

        }

        bool square_is_occupied (string coord_str) {
            /* Checks if provided square holds a piece. */
            return white_symbol_occupation_map.count(coord_str) + black_symbol_occupation_map.count(coord_str) > 0;
        }

        bool square_is_occupied_by_color (int color, string coord_str) {
            return get_occupation_map(color).count(coord_str) > 0;
        }

        bool square_is_occupied_by_enemy (int color, string coord_str) {
            /* Checks if provided square holds a enemy piece. 
            color: friendly color */
            // flip the color to search for enemy occupation map
            if (color == pieces.w)
                color = pieces.b;
            else
                color = pieces.w;
            return square_is_occupied_by_color (color, coord_str);
        }

        void update_king_scopes (int king_color) {
            
            /* Updates king scope variables.
            This method should be called in refresh method within an active move which was played.
            The algorithm will locate the new active king from which it will screen all directions
            for attackers, checks and denote pinned coordinates and space to a checking attacker
            i.e. a vector of empty in-between squares. */
            
            vector<std::vector<int>> offs; // 2D offset vector

            // reset global vectors for direct insertion, and get color
            string king_coord;
            if (king_color == pieces.w) {
                king_coord = white_king_coord;
                white_pins.clear();
                check_coords.clear();
                white_spaces.clear();
                black_scopers.clear();
            } else {
                king_coord = black_king_coord;
                black_pins.clear();
                check_coords.clear();
                black_spaces.clear();
                white_scopers.clear();
            }

            map<string, string> square = get_square_from_coord(king_coord);
            int rank = stoi(square["rank"]),
                file = stoi(square["file"]),
                r, f;

            string first_friendly_coord;
            char pointer_symbol;
            int pointer_piece;
            int pointer_color;
            string pointer;
            vector<string> spacing;
            bool is_checked = 0,
                 was_checked_in_this_iteration = 0;
            

            // from king coordinate iterate in all directions
            // integers 0: vertical, 1: horizontal, 2: diag
            for (int d = 0; d < 3; d++) {
                
                // determine the set of offset vectors 
                if (d == 0) 
                    offs = v_offset;
                else if (d == 1)
                    offs = h_offset;
                else 
                    offs = d_offset;
                
                // iterate through offset variations i.e. for vertical we have forward and backward
                for (int ind = 0; ind < offs.size(); ind++) {

                    // reset dynamic variables
                    first_friendly_coord = "";
                    spacing.clear();
                    was_checked_in_this_iteration = 0;

                    for (int step = 1; step < 7; step++) {
                        
                        // shift pointer by one offset step
                        r = rank + step * offs[ind][0];
                        f = file + step * offs[ind][1];
                        
                        // break offset direction search if exceeds the board
                        if (!inside_bounds(r, f))
                            break;

                        // compute pointer info
                        pointer = get_coord_from_file_and_rank(r, f);
                        pointer_symbol = get_symbol_from_coord(pointer);
                        pointer_piece = pieces.from_symbol(pointer_symbol);
                        
                        // skip empty pieces, but denote in spacing vector
                        if (pointer_piece == pieces.None) {
                            spacing.push_back(pointer);
                            continue;
                        }
                        
                        pointer_color = get_color_from_symbol(pointer_symbol);

                        // is occupied by enemy
                        if (pointer_color != king_color) {
                            
                            // check if the offset direction matches the possible attacker pieces
                            // e.g. vertical and horizontal: Rook and Queen
                            if ( ((d == 0 || d == 1) && (pointer_piece == pieces.Rook || pointer_piece == pieces.Queen)) ||
                                 ((d == 2) && (pointer_piece == pieces.Bishop || pointer_piece == pieces.Queen))
                            ) {
                                
                                // add to scopers list
                                if (king_color == pieces.w)
                                    black_scopers.push_back(pointer);
                                else
                                    white_scopers.push_back(pointer);
                                
                                // check if a friendly piece was spotted before-hand, this will become a pinned coord
                                if (first_friendly_coord != "") {

                                    if (king_color == pieces.w)
                                        // white_pins.push_back(first_friendly_coord);
                                        // it will turn out useful to remember the spacing around a pinned piece
                                        // i.e. the empty squares between king and pinned piece as well as
                                        // the ones between pinned piece and scoper coordinate.
                                        white_pins[first_friendly_coord] = spacing;
                                    else
                                        // black_pins.push_back(first_friendly_coord);
                                        black_pins[first_friendly_coord] = spacing;

                                } 
                                
                                // otherwise it must be a check
                                else {

                                    if (!is_checked)
                                        was_checked_in_this_iteration = 1;

                                    // denote check 
                                    is_checked = 1;
                                    check_coords.push_back(pointer);

                                }

                                // remember spacing squares between king and attacker globally,
                                // or king and pinned piece and pinned piece and attacker
                                if (step > 1 && was_checked_in_this_iteration) {
                                    if (king_color == pieces.w)
                                        white_spaces[pointer] = spacing; // ?
                                    else
                                        black_spaces[pointer] = spacing;
                                }
                                
                                // end here since an enemy piece was detected
                                break;

                            } 

                        }
                        
                        // or a friendly piece ..
                        else {

                            // .. which is a second friendly piece, then break
                            if (first_friendly_coord != "") 
                                break;
                            
                            // otherwise this is the first
                            first_friendly_coord = pointer;

                        }

                    }

                }

            }

            // check if the king is attacked by a knight
            for (int k = 0; k < n_offset.size(); k++) {
                
                // shift pointer by one offset step
                r = rank + n_offset[k][0];
                f = file + n_offset[k][1];
                
                // break offset direction search if exceeds the board
                if (!inside_bounds(r, f))
                    continue;

                // compute pointer info
                pointer = get_coord_from_file_and_rank(r, f);
                pointer_symbol = get_symbol_from_coord(pointer);
                pointer_piece = pieces.from_symbol(pointer_symbol);
                
                // verify it's a knight
                if (pointer_piece == pieces.Knight) {
                    
                    // denote check
                    is_checked = 1;
                    check_coords.push_back(pointer);

                    // only one knight can check at a time, so break
                    break;

                }

            }
            
            // finally probe if checked by pawn
            int forward_rank,
                curr_file;
            char pawn_symbol;
            if (king_color == pieces.w) {
                // rank+1 file
                forward_rank = rank + 1;
            } else {
                forward_rank = rank - 1;
            }
            for (int it = 0; it < 2; it++) {
                curr_file = file + pow(-1, it);
                if (!inside_bounds(forward_rank, curr_file)) {
                    // reuse pointer variables
                    pointer = get_coord_from_file_and_rank(curr_file, forward_rank);
                    pointer_symbol = get_symbol_from_coord(pointer);
                    pointer_color = get_color_from_symbol(pawn_symbol);
                    pointer_piece = pieces.from_symbol(pawn_symbol);
                    if (pointer_piece == pieces.Pawn && pointer_color != king_color) {
                        // pointer is enemy pawn.
                        // add the pawn to check coords, ignore spaces by nature.
                        check_coords.push_back(pointer);
                        is_checked = 1;
                        break;
                    }
                }
                    
            }
            

            // denote the check globally if it was detected
            if (is_checked)
                if (king_color == pieces.w)
                    white_is_checked = 1;
                else
                    black_is_checked = 1;
            else {
                white_is_checked = black_is_checked = 0;
            }

        }

        void update_targets_and_moves (int color) {

            /* Integrated and improved method which updates 
            targets and legal moves for a color. 
            Should be called after king scope was updated. */

            // init new empty dummy maps and variables
            map<string, vector<string>> target_map, move_map, pins, spaces;
            map<string, char> occupation_map = get_occupation_map(color);
            string coord;
            char symbol;
            vector<string> intersection, targets, moves;

            // first determine the targets for active color anyway
            // as those will be crucial for the next move (i.e. enemy) 
            for (auto const& x : occupation_map) {

                coord = x.first;
                symbol = x.second;

                // denote target coordinates
                vector<string> targets = reachable_target_coords(coord, symbol, color);
                target_map[coord] = targets;

                // first remove the non-legal pawn capture moves.
                // check if the diagonal pawn targets really hold enemies
                if (pieces.from_symbol(symbol) == pieces.Pawn) {
                    const char pawn_file = coord[0];
                    if (color == pieces.w)
                        pins = white_pins;
                    else
                        pins = black_pins;
                        
                    for (int i = 0; i < targets.size(); i++) {
                        // add target to moves if it is same file
                        if (pawn_file == targets[i][0]) {
                            if (!move_map.count(coord)) 
                                move_map[coord] = {};
                            move_map[coord].push_back(targets[i]); 
                        }
                        // if pawn is on another file check for enemies to be a valid move
                        // otherwise check if the target is an en-passant target
                        else if (square_is_occupied_by_enemy(color, targets[i]) ||
                                 en_passant_coord != "-" && targets[i] == en_passant_coord) {
                            move_map[coord].push_back(targets[i]);
                        }
                    }
                }

            }

            // check if provided color is in check
            if (color == pieces.w && white_is_checked || color == pieces.b && black_is_checked) {

                // if in check reset the move map, which at this point holds all legal pawn moves
                // move_map.clear();

                // get king coord
                string king_coord;
                map<string, vector<string>> enemys_targets;
                if (color == pieces.w) {
                    king_coord = white_king_coord;
                    enemys_targets = black_targets;
                } else {
                    king_coord = black_king_coord;
                    enemys_targets = white_targets;
                }

                // and king symbol from occupation mapping
                char king_symbol = occupation_map[king_coord];

                // draw targets for the attacked king, the targets were already computed
                vector<string> king_targets = target_map[king_coord]; 
                // reachable_target_coords(king_coord, king_symbol, color); targets are known already

                // determine king escape squares
                string king_target;
                vector<string> escape_coords;
                bool target_is_attacked;
                for (int i = 0; i < king_targets.size(); i++) {
                    target_is_attacked=0;
                    king_target = king_targets[i];
                    for (auto const& x : enemys_targets) {
                        // check if the targets include the king_target
                        if (contains_string(x.second, king_target)) {
                            target_is_attacked = 1;
                            break;
                        }
                    }
                    if (!target_is_attacked)
                        escape_coords.push_back(king_target);
                }

                // then check the number of checkers, if greater than 1 the king has to move
                // and we can neglect all other pieces for performance
                if (check_coords.size() > 1) {

                    // delete the move map as only the king will be able to move to escape 
                    move_map.clear();

                    // add the king escape coords only to valid move map
                    move_map[king_coord] = escape_coords;
                    
                    // analyze the resulting escape coords
                    if (!escape_coords.size()) {
                        // check mate as no escape routes are left
                        print(pieces.color_string(color) + " king has no escape routes, check mate!", "board");
                    }
                    
                }

                // otherwise the king is attacked only once and other friendly pieces may block the check
                else {

                    // determine the current pins and spaces for color
                    if (color == pieces.w) {
                        pins = white_pins;
                        spaces = white_spaces;
                    } else {
                        pins = black_pins;
                        spaces = black_spaces;
                    }

                    // if there is a check we need to revert to the yet added pawn moves and restrict them
                    for (auto const& x : move_map) {

                        coord = x.first;
                        moves = x.second;
                        
                        // skip if the pawn is pinned
                        if (pins.count(coord))
                            // this pawn has no moves, so erase from move_map
                            move_map.erase(coord);
                            continue;

                        // otherwise check if it can block the check
                        intersection = intersect(moves, spaces[0]);
                        if (intersection.size())
                            move_map[coord] = intersection;
                        else
                            // again this pawn has no moves, so erase from move_map
                            move_map.erase(coord);
                    }

                    // only now add the king escape coords to valid move map
                    move_map[king_coord] = escape_coords;

                    // iterate over all possible targets 
                    vector<string> targets;
                    for (auto const& x : occupation_map) {
                        
                        coord = x.first;
                        
                        // skip if the piece is pinned as it cannot
                        // come to help in that case
                        if (pins.count(coord))
                            continue;

                        symbol = x.second;

                        // vector<string> targets = reachable_target_coords(coord, symbol, color);
                        targets = target_map[coord]; 
                    
                        // denote legal moves if the piece can block the check
                        // i.e. intersect the line between scoper and king
                        if (spaces.count(check_coords[0])) {
                            intersection = intersect(targets, spaces[check_coords[0]]);
                            if (intersection.size())
                                move_map[coord] = intersection;
                        }

                        // check if the non-pinned piece can capture the only checking scoper
                        if (contains_string(targets, check_coords[0])) {
                            if (!move_map.count(coord))
                                move_map[coord] = {};
                            move_map[coord].push_back(check_coords[0]);
                        };
                        
                    }
                
                }
            
            } 
            
            // continue compute all moves
            else {

                if (color == pieces.w)
                    pins = white_pins;
                else
                    pins = black_pins;

                // check if the yet added pawn moves aren't pinned and restrict their moves further
                for (auto const& x : move_map) {

                    coord = x.first;
                    moves = x.second;
                    
                    // check if the pawn is pinned
                    if (pins.count(coord))
                        // select which moves are within the pins
                        intersection = intersect(moves, pins[coord]);
                        if (intersection.size())
                            move_map[coord] = intersection;
                        else
                            move_map.erase(coord);

                    // otherwise check if it can block the check
                    intersection = intersect(moves, spaces[0]);
                    if (intersection.size())
                        move_map[coord] = intersection;
                    else
                        // again this pawn has no moves, so erase from move_map
                        move_map.erase(coord);
                }

                vector<string> targets, space;
                for (auto const& x : occupation_map) {

                    coord = x.first;
                    symbol = x.second;

                    // denote target coordinates
                    targets = target_map[coord];
                    intersection.clear();

                    // if the coord/piece is pinned restrict targets vector
                    // as the pinned piece can only move along the pin space
                    if (color == pieces.w && white_pins.count(coord) ||
                        color == pieces.b && black_pins.count(coord)) {
                        
                        // the pinned piece can only move in the space 
                        // between attacker and king, so take intersection
                        if (color == pieces.w)
                            space = white_pins[coord];
                        else
                            space = black_pins[coord];

                        // denote allowed moves for the pinned piece, by intersecting the
                        // possible target set with the allowed pin space
                        // intersect the moves for pawns because the legal moves were 
                        // already sorted at te very beginning of this function
                        if (pieces.from_symbol(symbol) == pieces.Pawn) {
                            intersection = intersect(move_map[coord], space);
                        } 

                        // otherwise intersect the targets
                        else {
                            intersection = intersect(targets, space);
                        }

                        // only override the move map
                        if (intersection.size())
                            move_map[coord] = intersection;

                        // also check if the scoper can be captured by the pinned piece
                        if (color == pieces.w) {
                            intersection = intersect(targets, black_scopers); // ?
                        } else {
                            intersection = intersect(targets, white_scopers);
                        }

                        // if the scoper is included, add it's coord to move map
                        if (intersection.size()) {
                            if (move_map.count(coord))
                                move_map[coord] = {};
                            move_map[coord].push_back(intersection[0]);
                        }

                    }

                    // otherwise append targets as valid moves
                    else
                        move_map[coord] = targets;

                }

                // reset the target map
                if (color == pieces.w)
                    white_targets = target_map;
                else
                    black_targets = target_map;


            }

        }

};