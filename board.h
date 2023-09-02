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

using namespace std;


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

    */

    public:

        /* ---- General Parameters ---- */
        // Verbose mode which prints every operation
        bool __verbose__ = true; 


        // constructor sequence
        Board(void) {

            // initialize grid
            build_grid();
            
        };


        /* get methods */
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

            // retrieve rank from mapping the 2nd char to 
            // an integer.
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
            //cout << "hit 1" << endl;
            
            return grid[id];

        };

        char get_symbol_from_coord (string coord_str) {

            /* Returns the occupation symbol i.e. chess piece at the 
            desired coordinate. The return '_' indicates an empty square. */
            //cout << "hit 2 " << get_square_from_coord(coord_str)["symbol"][0] << " " << coord_str << endl;
            return get_square_from_coord(coord_str)["symbol"][0];

        };

        
        /* output methods */
        void show_active_color () {
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

        void show_material () {
            cout << "white's material count: " << count_material(pieces.w) << endl;
            cout << "black's material count: " << count_material(pieces.b) << endl;
        };

        void show_move_count () {
            cout << "move count: " << move_count << endl;
        };

        void show_position_activity () {
            cout << "white's position activity: " << count_position_activity(8) << endl;
            cout << "black's position activity: " << count_position_activity(16) << endl;
        }

        void show_move_count_for_active_color () {

            /* Shows the amount of moves the active color can play. */

            string col_str;
            if (active_color == pieces.w)
                col_str = "white";
            else
                col_str = "black";
            cout << "possible moves for " << col_str << ": " << count_moves(active_color) << endl;

        };

        void show_moves_for_active_color () {
            
            string  col_str,
                    target_string,
                    coord;
            vector<string> targets;
            map<string, vector<string>> m;

            if (active_color == pieces.w) {
                col_str = "white";
                m = moves_for_white;
            } else {
                m = moves_for_black;
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
            
            for (auto const& x : move_history) 
                cout << x.first << ". " << x.second[0] << " " << x.second[1] << endl;
        
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
                m = targets_for_white;
            } else {
                m = targets_for_black;
                col_str = "black";
            }

            cout << "reachable squares for " << col_str << ":" << endl;

            for (auto const& x : m) {

                coord = x.first;
                // show the square for the piece on coord
                show_reachable_squares (coord);

            }

        }


        /* manipulation/set methods */

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

        void clear () {

            /* Removes all pieces from the board. */

            for (int i = 0; i < 64; i++)
                grid[i]["symbol"] = '_';
            
        };

        void ignorant_move (string origin_coord_str, string target_coord_str, bool verbose=1) {

            /* Moves a piece disregarding chess rules by a combination of remove and place methods. 
            Open checks are disregarded, however additional en-passant and castling moves are accounted. 
            Also the move information are saved in global last move and capture variables. */

            // determine the piece color
            int color;
            char origin_symbol = get_square_from_coord(origin_coord_str)["symbol"][0];  // not proper
            if ( pieces.is_white(origin_symbol) ) 
                color = 8;
            else 
                color = 16;
            
            // if captures then apriori denote the enemy symbol at target coord
            if (square_is_occupied_by_enemy(color, target_coord_str)) {
                last_captured_symbol = get_symbol_from_coord(target_coord_str);
                last_captured_symbol_coord = target_coord_str;
            } else {
                last_captured_symbol = '_';
                last_captured_symbol_coord = "";
            }
            
            // determine piece
            int piece = pieces.from_symbol(origin_symbol);
            
            // remove the piece from origin
            remove_piece(origin_coord_str, verbose);

            // place the piece at new target
            place_piece(piece, color, target_coord_str, verbose);
            
            /* ---- Appendix Moves ---- */
            // check for castling, if so do another move with the rook
            if (piece == pieces.King) {
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
                    remove_piece(last_captured_symbol_coord, verbose);
                }
                
            }

            // override global variables but not game variables 
            // (since the move can be executed outside of a game)
            last_move[0] = origin_coord_str;
            last_move[1] = target_coord_str;
            
        };

        bool legal_move (string origin_coord_str, string target_coord_str, bool verbose) {

            /* Makes an ignorant move (without chaning gaming parameters) 
            only if legal, and returns a boolean. */
            
            if (!move_is_legal(origin_coord_str, target_coord_str)) {
                cout << "move " << origin_coord_str << " -> " << target_coord_str << " is not legal." << endl;
                // test for debugging
                show_board();
                return false;
            } 
            
            // now the main ignorant move is legal
            ignorant_move(origin_coord_str, target_coord_str, verbose);

            return true;
        };
        
        void load_position_from_fen (string fen, bool verbose=1) {

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

                    // place the piece
                    place_piece(piece, color, coord, verbose);

                    // increment file
                    file++;
                
                // check for active color & override
                } else if (pieces_completely_parsed && !active_color_parsed && (_char == 'b' || _char == 'w')) {
                    //cout << "active color test: " << _char << endl;
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
                        //cout << "test moves " << moves << endl;
                        move_count = stoi(moves);
                        if (verbose)
                            print("Successfully loaded position from FEN.", "Board");
                        // finished   
                    }
                } 

            };
        };
        
        void load_starting_position (bool verbose=1) {
            
            /* Loads all pieces to the board grid by using their 
            symbol values. Start position fen is loaded via fen parser.*/

            if (verbose)
                cout << "load starting position ..." << endl;
            load_position_from_fen(starting_position_fen, verbose);
            if (verbose)
                cout << "successfully loaded starting position." << endl;
        };

        void place_piece (int piece, int color, string coord_str, bool verbose=1) {

            /* Places a piece on the board */
            char piece_symbol = pieces.to_symbol(piece, color);

            if (verbose)
                cout << "place " << pieces.name_from_symbol(piece_symbol) << " (" << piece_symbol << ")" << " at " << coord_str << endl;
            
            set_symbol_at_coord(piece_symbol, coord_str);

        };

        void place_symbol (char symbol, string coord_str) {

            /* Just an alias for symbol placement. */

            set_symbol_at_coord(symbol, coord_str);

        };

        void reset () {

            /* Resets the board by loading the starting position. */

            clear();
            load_starting_position();

        };

        void remove_piece (string coord_str, bool verbose=1) {

            /* Removes a piece from requested coordinate */

            char piece_symbol = get_symbol_from_coord(coord_str);
            //cout << "hit 2" << endl;
            if (verbose)
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
            //cout << "symbol test " << grid[id]["symbol"] << endl;

        };

        void undo_ignorant_move () {

            /* Undos the last ignorant move. */
            // skip if there is no previous move defined
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
            //ignorant_move(origin, target, 0);

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

        /* game methods */
        MoveInfo active_move (string origin_coord_str, string target_coord_str, bool verbose=0) {

            /* Active moves manipulate the board and game parameters. */

            MoveInfo info;
            char symbol = get_symbol_from_coord(origin_coord_str);
            int color = get_color_from_symbol(symbol);
            char captured_symbol;
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

            // format the move before the board is altered
            string move_notation = move_to_pgn(origin_coord_str, target_coord_str);

            if (legal_move(origin_coord_str, target_coord_str, verbose)) {

                int piece = pieces.from_symbol(symbol);

                // denote all necessary information to info object
                info.legal = 1;
                info.origin = origin_coord_str;
                info.target = target_coord_str;
                info.en_passant_coord = en_passant_coord;
                if (color == pieces.w) {
                    info.targets = targets_for_white;
                    info.moves = moves_for_white;
                } else {
                    info.targets = targets_for_black;
                    info.moves = moves_for_black;
                }
                
                // if (en_passant_coord != "-")
                //     en_passant_was_on = en_passant_coord;
                // else 
                //     en_passant_was_on = "-";  

                // denote the castling rights if king or rook move (for undo)
                info.castling_right_k_b = castling_right_k_b;
                info.castling_right_q_b = castling_right_q_b;
                info.castling_right_k_w = castling_right_k_w;
                info.castling_right_q_w = castling_right_q_w;
                // if (piece == pieces.King || piece == pieces.Rook) {
                //     bool castling_right_k_w_before = castling_right_k_w;
                //     bool castling_right_k_b_before = castling_right_k_b;
                //     bool castling_right_q_w_before = castling_right_q_w;
                //     bool castling_right_q_b_before = castling_right_q_b;
                // }
                
                // denote if a new en-passant possibility arises from this move
                if (piece == pieces.Pawn) {

                    int rank_origin = stoi(get_square_from_coord(origin_coord_str)["rank"]);
                    map<string, string> square = get_square_from_coord(target_coord_str);
                    int file = stoi(square["file"]),
                        rank = stoi(square["rank"]);
                    string coord;
                    char symbol;

                    // finally check if the pawn is moved from origin square for two ranks at once
                    if ((active_color == pieces.w && rank_origin == 1 && rank == 3) || (active_color == pieces.b && rank_origin == 6 && rank == 4)) {
                        
                        // check if a new en-passant possibility pops up for enemy
                        int sign = 1, en_passant_target_rank;
                        for (int x = 0; x < 2; x++) {
                            sign *= -1;
                            if (square_is_valid(rank, file+sign)) {
                                coord = get_coord_from_file_and_rank(file+sign, rank);
                                symbol = get_symbol_from_coord(coord);
                                if (square_is_occupied_by_enemy(active_color, coord) && pieces.from_symbol(symbol) == pieces.Pawn) {
                                    string en_passant_target_rank_str = "3";
                                    if (color == pieces.b)
                                        en_passant_target_rank_str = "5";
                                    // finally merge and override en passant coord
                                    en_passant_coord = square["file"] + en_passant_target_rank_str;

                                    break;
                                }
                            }
                        }

                    }
                } 
                
                // check if King moved to remove castling rights
                else if (piece == pieces.King) 
                    if (color == pieces.w) 
                        castling_right_k_w = 0, castling_right_q_w = 0;
                    else if (color == pieces.b) 
                        castling_right_k_b = 0, castling_right_q_b = 0;
                    

                // check if a rook moved to remove castling right
                else if (piece == pieces.Rook) 
                    if (origin_coord_str == "H1")
                        castling_right_k_w = 0;
                    else if (origin_coord_str == "A1")
                        castling_right_q_w = 0;
                    else if (origin_coord_str == "A8")
                        castling_right_q_b = 0;
                    else if (origin_coord_str == "H8")
                        castling_right_k_b = 0;

                // denote the legal move
                if (color == pieces.w)
                    move_history[move_count] = {};
                move_history[move_count].push_back(move_notation);

                // override move count only if black has played and alternate active color
                info.move_count = move_count;
                if (color == pieces.b) {
                    move_count++;
                    active_color = pieces.w;
                } else {
                    active_color = pieces.b;
                }

                // refresh the board
                refresh_position();

            }

            return info;
        }   

        void active_undo_from_info (MoveInfo info) {

            /* Unmakes the last active move. works on single depth only. */
            
            // draw the last move securely from persitent object
            string  origin = info.target,
                    target = info.origin;
            
            char symbol = info.symbol;
            int piece = pieces.from_symbol(symbol);
            
            // switch back active color
            active_color = info.color;
            move_count = info.move_count;

            // reset castling rights
            castling_right_k_w = info.castling_right_k_w;
            castling_right_k_b = info.castling_right_k_b;
            castling_right_q_w = info.castling_right_q_w;
            castling_right_q_b = info.castling_right_q_b;
            
            // bring back moves and targets for origin position
            if (active_color == pieces.w) {
                targets_for_white = info.targets;
                moves_for_white = info.moves;
            } else {
                targets_for_black = info.targets;
                moves_for_black = info.moves;
            }
            
            // remove the move from PGN notation
            if (move_history.count(move_count))
                move_history[move_count].pop_back();
            else
                move_history[move_count-1].pop_back();
            
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

            // reset the captured piece
            if (info.capture_coord != "") 
                place_symbol(info.capture_symbol, info.capture_coord);
            
            // revert main move
            remove_piece(info.target, 0);
            place_symbol(info.symbol, info.origin);
            
        };

        map<string, vector<string>> get_possible_moves_for_active_color () {
            if (active_color == pieces.w)
                return moves_for_white;
            return moves_for_black;
        }

        void refresh_position () {

            /* Main parsing mechanism for position and map computation. 
            Should be called after every board alternation e.g. at the end of an active move. 
            The method works color-wise for efficiency reasons, and will gather
            all targets, moves and symbol mappings, for global access. */

            // keep the recent moves and targets persistent
            // update target map and move object depending on color
            if (active_color == pieces.w) {
                old_moves = moves_for_white;
                old_targets = targets_for_white;
                update_moves_from_targets(update_reachable_target_map(pieces.w), pieces.w);
            } else {
                old_moves = moves_for_black;
                old_targets = targets_for_black;
                update_moves_from_targets(update_reachable_target_map(pieces.b), pieces.b);
            }

            // map all symbols to their current coordinate
            update_symbol_map();

        }

        /* game implementation */
        void game () {

            // intro
            cout << "New Game!" << endl;

            while (true) {

                update_moves_from_targets(update_reachable_target_map(active_color), active_color);

                break;    

            }
        }     
        
    private:

        /*   Define global variables   */
        // prepare letters for the board files
        const char letter_coordinates[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
        const string starting_position_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        

        // castling rights
        bool castling_right_k_w = 1;
        bool castling_right_k_b = 1;
        bool castling_right_q_w = 1;
        bool castling_right_q_b = 1;
        bool castling_right_k_w_before;
        bool castling_right_k_b_before;
        bool castling_right_q_w_before;
        bool castling_right_q_b_before;

        
        /* Denote everything for the next move i.e. from current position 
            map ->  attacker_coord: [attacking_coord_1, 2, ...]
                    attacker_coord: [attacking_coord_1, 2, ...]
                    ...
        */
        map<string, vector<string>> targets_for_white,
                                    targets_for_black,
                                    moves_for_white,
                                    moves_for_black,
                                    old_targets,
                                    old_moves;

        // define relative decrementation coordinates, which persist
        vector<vector<int>> n_coords {{1,2}, {2,1}, {-1,2}, {-2,1}, {-1,-2}, {-2,-1}, {1,-2}, {2,-1}}; // knight relative coordinates
        vector<vector<int>> d_coords {{1,1}, {1,-1}, {-1,-1}, {-1,1}};
        vector<vector<int>> v_coords {{1,0}, {-1,0}};
        vector<vector<int>> h_coords {{0,1}, {0,-1}};  

        // The board grid maps the square IDs (0 ... 63) => square info.*/
        map<string, string> grid[64];

        // also define a map symbol -> coord(symbol)
        map<char, vector<string>> coord_symbol_map;

        // Load the pieces
        Piece pieces;

        // Init game parameters
        int active_color = pieces.w;
        /* while the last_move object is for quick iteration this  
        will be empty after position refresh
        while the persistent object remains. */ 
        string last_move[2] = {"", ""};
        string last_move_persistent[2] = {"", ""};
        char last_captured_symbol = '_';
        string last_captured_symbol_coord = "";
        map<int, vector<string>> move_history;

        // fen parameters
        string en_passant_coord = "-";
        string en_passant_was_on = "-";
        int half_clock = 0;
        int move_count = 1;

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
                    etc.                                            */
            
            
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
        bool can_castle_king_side (int color) {
            
            /* Checks if a color can castle king side. */

            if (!is_checked(color)) {
                if (color == pieces.w && castling_right_k_w) {
                    if (!square_is_occupied("F1") && !square_is_occupied("G1") && !square_is_attacked("F1", color) && !square_is_attacked("G1", color))
                        return 1;
                    
                } else if (color == pieces.b && castling_right_k_b) {
                    if (!square_is_occupied("F8") && !square_is_occupied("G8") && !square_is_attacked("F8", color) && !square_is_attacked("G8", color))
                        return 1;
                }
            }

            return 0;

        };

        bool can_castle_queen_side (int color) {

            /* Checks if a color can castle queen side. */

            if (!is_checked(color)) {
                if (color == pieces.w && castling_right_q_w) {
                    if (!square_is_occupied("C1") && !square_is_occupied("D1") && !square_is_attacked("C1", color) && !square_is_attacked("D1", color))
                        return 1;
                    
                } else if (color == pieces.b && castling_right_q_b) {
                    if (!square_is_occupied("C8") && !square_is_occupied("D8") && !square_is_attacked("C8", color) && !square_is_attacked("D8", color))
                        return 1;
                }
            }

            return 0;

        };

        string move_to_pgn (string origin_coord_str, string target_coord_str) {
            
            /*
            Converts a move to portable game notation string, depending on board postion etc.
            This function should be called within the active move function but before the ignorant move within is played,
            as this would alter the board position.
            */

            map<string, string> origin_square = get_square_from_coord(origin_coord_str);
            char origin_symbol = origin_square["symbol"][0];
            char symbol_cap = toupper(origin_symbol);
            int color = get_color_from_symbol(origin_symbol);
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
            if (square_is_occupied_by_enemy(color, target_coord_str))
                formatted = 'x' + formatted;
            
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
            if (color == pieces.w)
                m = moves_for_white;
            else
                m = moves_for_black;

            // iterate through all other attackers of origin color, if another attacker
            // is the same piece as the origin piece, we need to distinguish either the file or rank
            map<string, string> attacker_square;
            int attacker_rank, attacker_file, attacker_piece;
            for (auto const& x : m) {
                // if attacker square is different from origin proceed
                if (x.first != origin_coord_str) {
                    
                    // if the other attacker square is attacking the target as well proceed
                    if (contains_string(x.second, target_coord_str)) {

                        attacker_coord = x.first;
                        attacker_square = get_square_from_coord(attacker_coord);
                        attacker_rank = stoi(attacker_square["rank"]);
                        attacker_file = stoi(attacker_square["file"]);
                        attacker_piece = pieces.from_symbol(attacker_square["symbol"][0]);
                        
                        // if the attacking piece is no pawn and is one of two pieces of same kind which attack add the file
                        if (attacker_piece == origin_piece) {

                            int origin_file = stoi(origin_square["file"]);
                            
                            // if on same file denote the rank
                            if (attacker_file == origin_file) 
                                return symbol_cap + lower_case(origin_coord_str[1] + formatted);

                            // otherwise denote file
                            return symbol_cap + lower_case(origin_file_str + formatted);

                        }

                    }
                }
                
            }

            return symbol_cap + lower_case(formatted);

        };

        bool move_is_legal (string origin_coord_str, string target_coord_str) {
            
            /* Checks if a move is legal by general chess rules.
            The playable moves from origin are drawn from the targets map. */

            vector<string> playable_moves;
            char symbol = get_symbol_from_coord(origin_coord_str);
            int piece = pieces.from_symbol(symbol);
            int origin_color = get_color_from_symbol(symbol);
            

            // check if active color is respected
            if (origin_color == 0)
                return false;
            else if (origin_color != active_color) {
                string col_str;
                if (active_color == pieces.w) 
                    col_str = "white";
                else 
                    col_str = "black";
                cout << "it is " << col_str << "'s turn!" << endl;
                return false;
            }
           
            // first check if move is a castling move and if it is valid, otherwise continue
            if (piece == pieces.King) {
                if (origin_color == pieces.w && origin_coord_str == "E1" && target_coord_str == "G1") {
                    if (can_castle_king_side(origin_color))
                        return true;
                    else
                        return false;
                }
                if (origin_color == pieces.b && origin_coord_str == "E8" && target_coord_str == "G8") {
                    if (can_castle_king_side(origin_color))
                        return true;
                    else
                        return false;
                }
                if (origin_color == pieces.w && origin_coord_str == "E1" && target_coord_str == "C1") {
                    if (can_castle_queen_side(origin_color))
                        return true;
                    else
                        return false;
                } 
                if (origin_color == pieces.b && origin_coord_str == "E8" && target_coord_str == "C8") {
                    if (can_castle_queen_side(origin_color))
                        return true;
                    else
                        return false;
                } 
            }
             
            // en-passant
            // was checked already in reachable_targets object, it only needs to be checked if the en-passant
            // move leaves an open check which will be done by default in the following.

            // make sure there is no check after move (possible bottleneck)
            // not sure if this is actually needed since the moves object contains the moves which leave no open check.
            // if (move_leaves_open_check(origin_coord_str, target_coord_str))
            //     return false;
            // insted try to find the corresponding move in pre-computed move object
            playable_moves = moves_for_black[origin_coord_str];
            if (origin_color == pieces.w)
                playable_moves = moves_for_white[origin_coord_str];
            if (contains_string(playable_moves, target_coord_str))
                return true;
            
            return false;
            
        };

        bool move_leaves_open_check (string origin_coord_str, string target_coord_str) {
            
            /* Checks by quick simulation if a move leaves an open check, i.e. is ultimately pinned. */

            int target_color,
                origin_color = get_color_from_symbol(get_symbol_from_coord(origin_coord_str));
            if (origin_color == pieces.w)
                target_color = pieces.b;
            else
                target_color = pieces.w;
            
            // simulate
            bool result;
            ignorant_move(origin_coord_str, target_coord_str, 0);
            if (is_checked(origin_color))
                result = true;
            else
                result = false;
            
            // revert position, the undo function will take care about castling and en-passant appendix moves
            // to do this quickly it uses the last move and capture information.
            undo_ignorant_move();
                
            return result;
            
        };

        vector<string> reachable_target_coords (string coord_str) {

            /* 
            Returns a string vector of reachable target coordinates. 
            If the coord_str is empty ('_') the output vector will be empty as well.
            */

            vector<string> out = {};
            map <string, string> square = get_square_from_coord(coord_str);
            char symbol = square["symbol"][0];
            
            // cut short in case the square is empty
            if (symbol == '_') {return out;}

            vector<vector<int>> decrement;
            int color = get_color_from_symbol(symbol);
            int piece = pieces.from_symbol(symbol);

            string target_coord;
            int rank = stoi(square["rank"]),
                file = stoi(square["file"]);

            
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
                        if (!square_is_valid(file, rank+step*sign)) 
                            break; 
                        target_coord = get_coord_from_file_and_rank(file, rank+step*sign);
                        if (square_is_occupied(target_coord))
                            break;
                        out.push_back(target_coord);
                    }

                    /* captures */
                    int s = -1;
                    for (int i = 0; i < 2; i++) {
                        s *= -1;
                        if (square_is_valid(file+s, rank+sign)) {
                            target_coord = get_coord_from_file_and_rank(file+s, rank+sign);
                            if (square_is_occupied_by_enemy(color, target_coord))
                                out.push_back(target_coord);
                        }
                    }
                    // for (int i = 0; i < 2; i++) {
                    //     if (square_is_valid(file+pow(-1.,i), rank+sign)) {
                    //         target_coord = get_coord_from_file_and_rank(file+pow(-1.,i), rank+sign);
                    //         if (square_is_occupied_by_enemy(color, target_coord))
                    //             out.push_back(target_coord);
                    //     }
                    // }
                    
                    /* en-passant */
                    if (en_passant_coord != "-") {
                        // check if the selected pawn is next to en-passant coord
                        map <string, string> ep_square = get_square_from_coord(en_passant_coord); 
                        int ep_file = stoi(ep_square["file"]);
                        if (rank == stoi(ep_square["rank"])-sign && (file+1 == ep_file || file-1 == ep_file))
                            out.push_back(en_passant_coord);
                    }
                    
                } else if (screeningDirections[i] == 'n') { // edge case for knight
                    int r, f;
                    for (int i = 0; i < 8; i++) {
                        r = rank + n_coords[i][0], 
                        f = file + n_coords[i][1];
                        if (square_is_valid(r, f)) {
                            target_coord = get_coord_from_file_and_rank(f, r);
                            if (!square_is_occupied(target_coord) || square_is_occupied_by_enemy(color, target_coord))
                                out.push_back(target_coord);
                        }
                    }
                } else if (screeningDirections[i] == 'd')   // load diagonal decrementation coords
                    decrement = d_coords;
                else if (screeningDirections[i] == 'v')   // load vertical decrementation coords
                    decrement = v_coords;
                else if (screeningDirections[i] == 'h')   // load horizontal decrementation coords
                    decrement = h_coords;

                // decrementation algorithm for v, h, d
                if (screeningDirections[i] == 'v' || screeningDirections[i] == 'h' || screeningDirections[i] == 'd') {
                    int r, f, steps = 8;
                    if (piece == pieces.King)
                        steps = 2;
                    // iterate direction
                    for (int i = 0; i < decrement.size(); i++) {
                        for (int step = 1; step < steps; step++) {
                            r = rank + step*decrement[i][0],
                            f = file + step*decrement[i][1];
                            if (square_is_valid(r, f)) {
                                target_coord = get_coord_from_file_and_rank(f,r);
                                if (square_is_occupied_by_enemy(color, target_coord)) {
                                    out.push_back(target_coord);
                                    break;
                                } else if (!square_is_occupied(target_coord))
                                    out.push_back(target_coord);
                                else
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

        vector<string> square_is_attacked_by_coords (string coord_str, map<string, vector<string>> enemy_targets) {

            /* Returns all enemy (attacker) coordinates which are hitting the square. 
            The coordinates are determined from the provided enemy target map.*/

            string attacker_coord;
            vector<string> attacking_coords;
            vector<string> out = {};

            for (auto const& x : enemy_targets) {

                // coordinate of attacker
                attacker_coord = x.first;

                // coordinates being attacked
                attacking_coords = x.second;

                for (int i = 0; i < attacking_coords.size(); i++) {
                    if (attacking_coords[i] == coord_str) {
                        out.push_back(attacker_coord);
                        break;
                    }
                }
                
            }

            return out;
            
        };

        bool is_checked (int color) {

            /* Returns a boolean dep. on wether the color is in check. */
            
            char symbol;
            string coord;
            bool breaker = false;

            // iterate through board to find the king's position
            if (color == pieces.w) {

                // start from 1st rank
                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        coord = get_coord_from_file_and_rank(i, j);
                        symbol = get_symbol_from_coord( coord );
                        if (symbol == 'K') {
                            breaker = true;
                            break;
                        }
                    }
                    if (breaker)
                        break;
                }
                
            } else {

                // start from 8-th rank
                for (int i = 7; i >= 0; i--) {
                    for (int j = 0; j < 8; j++) {
                        coord = get_coord_from_file_and_rank(i, j);
                        symbol = get_symbol_from_coord( coord );
                        if (symbol == 'k') {
                            breaker = true;
                            break;
                        }
                    }
                    if (breaker)
                        break;
                }

            }

            // check if king's coordinate is being attakcked
            vector<string> attacker_coords;
            if (color == pieces.w)
                attacker_coords = square_is_attacked_by_coords(coord, targets_for_black);
            else
                attacker_coords = square_is_attacked_by_coords(coord, targets_for_white);

            if (attacker_coords.size() == 0)
                return false;
            else
                return true;
        }

        bool square_is_attacked (string coord_str, int color) {

            /* Uses square_is_attacked_by_coords() to check if the returned attacker array is empty.
            If there are no attackers i.e. enemy pieces reaching this square, the return will be 0 (false), otherwise 1 (true). */
            
            map<string, vector<string>> m;
            vector<string> attackers;
            if (color == pieces.w) {
                m = targets_for_black;
            } else if (color == pieces.b) {
                m = targets_for_white;
            }

            attackers = square_is_attacked_by_coords(coord_str, m);

            if (attackers.size() == 0)
                return 0;
            return 1;

        };

        bool square_is_in_line (string coord1, string coord2, string probe_coord) {

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

        bool square_is_occupied (string coord_str) {
            return get_symbol_from_coord(coord_str) != '_';
        }

        bool square_is_occupied_by_enemy (int friendly_color, string coord_str) {

            /* Returns boolean-like integer. The return is 1 if the square is occupied by enemy piece otherwise 0 */

            char target_symbol = get_symbol_from_coord(coord_str);
            int target_color = get_color_from_symbol(target_symbol); 

            return target_color != friendly_color && target_color != 0;

        }

        bool square_is_occupied_by_friendly_piece (int friendly_color, string coord_str) {
            
            /* Returns bool dep. on wether a friendly piece persists on this coordinate. */

            char target_symbol = get_symbol_from_coord(coord_str);
            int target_color = get_color_from_symbol(target_symbol);

            return target_color == friendly_color;

        }

        bool square_is_valid (int rank, int file) {

            /* Checks if the rank and file integer provided map to a valid chess square. */

            // simply check if the indices lie within the boundaries
            if (rank >= 0 && rank < 8 && file >= 0 && file < 8)
                return 1;
            else
                return 0;
            
        };

        bool squares_are_aligned (string coord1, string coord2) {
            /* Check if the squares are aligned in any way. */
            if (squares_are_aligned_diagonally(coord1, coord2) || squares_are_aligned_horizontally(coord1, coord2) || squares_are_aligned_vertically(coord1, coord2))
                return 1;
            return 0;
        }

        bool squares_are_aligned_diagonally (string coord1, string coord2) {
            int r1 = stoi(get_square_from_coord(coord1)["rank"]);
            int r2 = stoi(get_square_from_coord(coord2)["rank"]);
            int f1 = stoi(get_square_from_coord(coord1)["file"]);
            int f2 = stoi(get_square_from_coord(coord2)["file"]);
            if (abs(r1-r2) == abs(f1-f2))
                return 1;
            return 0;
        };

        bool squares_are_aligned_horizontally (string coord1, string coord2) {
            int r1 = stoi(get_square_from_coord(coord1)["rank"]);
            int r2 = stoi(get_square_from_coord(coord2)["rank"]);
            if (r1 == r2)
                return 1;
            return 0;
        };

        bool squares_are_aligned_vertically (string coord1, string coord2) {
            int f1 = stoi(get_square_from_coord(coord1)["file"]);
            int f2 = stoi(get_square_from_coord(coord2)["file"]);
            if (f1 == f2)
                return 1;
            return 0;
        }

        map<string, vector<string>> update_reachable_target_map (int color) {

            /* Updates reachable target map for provided color and 
            overrides the persistent global variable targets_for_white/black. 
            The overriden map will be returned at the end. */

            string coord;
            map<string, vector<string>> m;
            
            // iterate through field
            for (int i = 0; i < 64; i++) {
                coord = get_coord_from_id(i);
                if (square_is_occupied_by_friendly_piece(color, coord)) {
                    // override/create entry for coord with vector of all reachable targets from that square
                    m[coord] = reachable_target_coords(coord);
                } 
            }
            
            // override & return
            if (color == pieces.w) {
                targets_for_white = m;
                return targets_for_white;
            } else {
                targets_for_black = m;
                return targets_for_black;
            }

        }

        map<string, vector<string>> update_moves_from_targets (map<string, vector<string>> target_map, int color) {
            
            /* Filters the legal moves from target map and overrides the moves object. */
            
            string origin_coord;
            vector<string> targets;
            map<string, vector<string>> moves;

            for (auto const& x : target_map) {
                origin_coord = x.first;
                targets = x.second;
                moves[origin_coord] = {};
                for (int i = 0; i < targets.size(); i++) {
                    if (!move_leaves_open_check(origin_coord, targets[i]))
                        moves[origin_coord].push_back(targets[i]);
                }
            }
            
            // override
            if (color == pieces.w) {
                moves_for_white = moves;
                return moves_for_white;
            } else {
                moves_for_black = moves;
                return moves_for_black;
            }
            
        }

        map<char, vector<string>> update_symbol_map () {

            /* Maps pieces to coordinates in current position. 
            Overrides global variable coord_symbol_map */

            char symbol;
            string coord;
            map<char, vector<string>> m;
            vector<string> coord_array[8];

            for (int i = 0; i < 64; i++) {

                symbol = grid[i]["symbol"][0];
                coord = grid[i]["coordinate"];

                if (symbol != pieces.None) {

                    if (m.find(symbol) == m.end())
                        m[symbol] = {};

                    m[symbol].push_back(coord);
                        
                }

            }

            // override
            coord_symbol_map = m;

            return coord_symbol_map;

        };

        /* Evaluation */
        int count_material (int color) {

            /* Counts material i.e. value of all pieces for a specific color */

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
        };

        int count_moves (int color) {

            /* Counts the possible moves from current position. */

            int count = 0;
            string attacker_coord;
            vector<string> attacking_coords;
            
            if (color == pieces.w) {
                for (auto const& x : targets_for_white) {
                    count += x.second.size();
                }
            } else {
                for (auto const& x : targets_for_black) {
                    count += x.second.size();
                }
            }

            return count;
        };

        int count_position_activity (int color) {

            /* Counts the activity of all pieces of a certain color.
            The positional validation is drawn from the Piece.positional_weight_map 
            and convolved with the material value. */

            char symbol;
            int piece;
            int white;
            float value = 0;
            float value_float; 
            string coord;

            for (int i = 0; i < 64; i++) {
                symbol = grid[i]["symbol"][0];
                white = pieces.is_white(symbol);
                coord = grid[i]["coordinate"];
                if (white && color == 8 || !white && color == 16 )
                    value += pieces.value_from_symbol(symbol) * position_activity(symbol, coord);
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
