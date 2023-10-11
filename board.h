#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <regex>

// include objects and libraries
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

        // ---- new method set ----

        // Load the pieces
        Piece pieces;

        int active_color = pieces.w;

        // prepare letters for the board files
        const char file_letters[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
        const char number_coordinates[8] = {'1', '2', '3', '4', '5', '6', '7', '8'};
        const string starting_position_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

        // also define a map symbol -> coord(symbol)
        // map<char, vector<string>> coord_symbol_map;
        map<string, char> white_symbol_occupation_map; // maps coord to symbol
        map<string, char> black_symbol_occupation_map;

        // game tracking 
        map<int, vector<string>> pgn_history;
        int full_moves = 1,
            half_clock = 0;

        // pawns
        string en_passant_coord = "-";

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

        string board_ascii_template;


        // constructor sequence
        Board(void) {

            print("Initialize ...", "board");
            load_board_ascii();

        }

        void load_board_ascii () {

            /* Loads the ascii board template from file to string variable. */

            std::ifstream inFile;
            inFile.open("board_ascii_template.txt"); //open the input file

            std::stringstream strStream;
            strStream << inFile.rdbuf(); //read the file
            std::string str = strStream.str(); //str holds the content of the file

            std::cout << str << "\n";
            board_ascii_template = str;

        }

        void show_board () {

            /* Shows the current board state in the console. 
            The output is build from the board ascii template. */

            string coord,
                   board_ascii = board_ascii_template,
                   symbol, 
                   square_number;
            int ind = 0;

            for (int r = 0; r < 8; r++) {   

                for (int f = 0; f < 8; f++) {   
                    
                    // declare square number for all 64 squares
                    ind++;
                    if (ind < 10)
                        square_number = "0" + to_string(ind);
                    else
                        square_number = to_string(ind);
                    coord = get_coord_from_file_and_rank(f, r);

                    // decide wether the square is occupied by white, black, or none
                    if (white_symbol_occupation_map.count(coord))
                        symbol = pieces.to_unicode(white_symbol_occupation_map[coord]) + ' ';
                    else if (black_symbol_occupation_map.count(coord)) 
                        symbol = pieces.to_unicode(black_symbol_occupation_map[coord]) + ' ';
                    else 
                        symbol = "  ";
                    // print(symbol, "*TEST*");
                    // override the board_ascii string by overriding the square number occurence with symbol
                    board_ascii = regex_replace(board_ascii, regex(square_number), symbol);
                
                }

            }
            cout << board_ascii << endl;

        }

        void show_moves (int color) {

            /* Outputs all moves for active color. 
            Key: origin coordinate
            Value: vector of possible move coordinates. */
            
            string  col_str,
                    target_string,
                    coord;
            vector<string> targets;
            map<string, vector<string>> m;

            if (color == pieces.w) {
                m = white_moves;
            } else {
                m = black_moves;
            }

            print("moves for " + pieces.color_string(color), "board");
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

        void show_occupation (int color) {
            print("Occupation map for " + pieces.color_string(color), "board");
            for (const auto& x: get_occupation_map(color)) {
                cout << x.first << ": " << x.second << endl;
            }
        }

        void show_targets (int color) {

            /* Outputs all targets for active color. 
            Key: origin coordinate
            Value: vector of target coordinates. */
            
            string  col_str,
                    target_string,
                    coord;
            vector<string> targets;
            map<string, vector<string>> m;

            if (color == pieces.w) {
                m = white_targets;
            } else {
                m = black_targets;
            }

            print("targets for " + pieces.color_string(color), "board");
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

            return file_letters[file] + to_string(rank+1); // faster method

        };

        vector<int> get_file_and_rank_from_coord (string coord_str) {
            /* Returns a 2d vector with file and rank (between 0 and 7). */
            vector<int> out = {};
            for (int i = 0; i < 8; i++) {
                if (file_letters[i] == coord_str[0]) {
                    out.push_back(i);
                    break;
                }
            }
            int new_rank = coord_str[1] - '0';
            new_rank--;
            out.push_back(new_rank);
            // cout  << "TEST: " << out[0] << " " << out[1] << endl;
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

            /* Removes all pieces from the board and resets all parameters. */

            white_symbol_occupation_map.clear();
            black_symbol_occupation_map.clear();
            white_moves.clear();
            black_moves.clear();
            white_pins.clear();
            black_pins.clear();
            white_spaces.clear();
            black_spaces.clear();
            white_targets.clear();
            black_targets.clear();
            white_pins.clear();
            pgn_history.clear();
            full_moves = 0;
            
        };

        void load_position_from_fen (string fen, bool verbose=1) {

            /* A fen parsing implementation which generates a position from compact string.
            The reading starts from upper left corner i.e. rank is decremented while the files
            are iterated from left to right. */

            int piece, 
                color, 
                file = 0, 
                rank = 7, 
                pieces_completely_parsed = 0,
                active_color_parsed = 0,
                castling_parsed = 0,
                en_passant_parsed = 0,
                half_clock_parsed = 0,
                delimiter_index,
                full_moves_parsed = 0;
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
                        file = 0;
                        continue;
                    // if integer is parsed shift file
                    } else if (isdigit(_char)) {
                        // integers account for file shifts
                        file += _char - '0' - 1;
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
                } else if (half_clock_parsed && !full_moves_parsed) {
                    moves += _char;
                    if (i == fen.size()-1) {
                        full_moves = stoi(moves);
                        if (verbose)
                            print("Successfully loaded position from FEN.", "Board");
                        // finished   
                    }
                } 

            };
            
            // initialize all possible moves for white
            update_king_scopes(active_color);
            update_targets_and_moves(active_color);
            
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
                // move not contained in allowed moves
                if (!contains_string(moves[origin_coord_str], target_coord_str)) {
                    print(pieces.color_string(color) + "'s move " + origin_coord_str + "->" + target_coord_str + " is not legal.", "board");
                    return snap;
                }
                // target square is occupied by a friendly piece
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

            /* ---- appendix moves ---- */

            // check if pawn move leaves an en-passant possibility
            if (piece == pieces.Pawn) {

                int rank_origin = get_file_and_rank_from_coord(origin_coord_str)[1];
                vector<int> fr = get_file_and_rank_from_coord(target_coord_str);
                int target_file = fr[0], target_rank = fr[1];

                // 1) check if en-passant is captured and remove the captured pawn
                if (target_coord_str == en_passant_coord) {
                    vector<int> fr_en_passant = get_file_and_rank_from_coord(en_passant_coord);
                    string capture_coord;
                    char captured_pawn_symbol;
                    if (color == pieces.w)
                        captured_pawn_symbol = 'p';
                    else
                        captured_pawn_symbol = 'P';
                    if (fr_en_passant[1] == 2) 
                        capture_coord = get_coord_from_file_and_rank(fr_en_passant[0], fr_en_passant[1]+1);
                    else
                        capture_coord = get_coord_from_file_and_rank(fr_en_passant[0], fr_en_passant[1]-1);
                    remove_symbol_at_coord(captured_pawn_symbol, capture_coord);
                }
                
                // 2) finally check if the pawn is moved from origin square for two ranks at once
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
                                en_passant_coord = file_letters[target_file] + en_passant_target_rank_str;

                                break;

                            }
                        
                        }

                    }

                } 
                
                // 3) otherwise erase the en-passant coordinate
                else

                    en_passant_coord = '-';

            } 
            
            // check if King moved
            else if (piece == pieces.King) {

                // looses castling rights when king is moved
                // and also save the king's position
                if (color == pieces.w) {
                    castling_right_k_w = 0, castling_right_q_w = 0;
                    white_king_coord = target_coord_str;
                } else if (color == pieces.b) {
                    castling_right_k_b = 0, castling_right_q_b = 0;
                    black_king_coord = target_coord_str;
                } 
                    
                // check if castles was played to append the corr. rook move
                if (color == pieces.w && origin_coord_str == "E1" && target_coord_str == "G1") {

                    // first check if castling is allowed
                    remove_symbol_at_coord('R', "H1");
                    place_symbol_at_coord('R', "F1");
                    // remove all castling rights for white
                    castling_right_k_w = castling_right_q_w = 0;

                } else if (color == pieces.w && origin_coord_str == "E1" && target_coord_str == "C1") {

                    // first check if castling is allowed
                    remove_symbol_at_coord('R', "A1");
                    place_symbol_at_coord('R', "D1");
                    // remove all castling rights for white
                    castling_right_k_w = castling_right_q_w = 0;

                } else if (color == pieces.b && origin_coord_str == "E8" && target_coord_str == "G8") {

                    // first check if castling is allowed
                    remove_symbol_at_coord('R', "H8");
                    place_symbol_at_coord('R', "F8");
                    // remove all castling rights for white
                    castling_right_k_b = castling_right_q_b = 0;

                } else if (color == pieces.b && origin_coord_str == "E8" && target_coord_str == "C8") {

                    // first check if castling is allowed
                    remove_symbol_at_coord('R', "A8");
                    place_symbol_at_coord('R', "D8");
                    // remove all castling rights for white
                    castling_right_k_b = castling_right_q_b = 0;

                }
                
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
            
            // check if castling rights might be lost due to the move
            if (!all_castling_rights_lost && (piece == pieces.King || piece == pieces.Rook)) {
                if (castling_right_k_w + castling_right_q_w + castling_right_k_b + castling_right_q_b == 0) {
                    all_castling_rights_lost = 1;
                }
            }

            // denote the move and add to history
            record_move(origin_coord_str, target_coord_str, symbol, color, captured_symbol);

            // update board
            tap_clock();
            flip_active_color();
            update_king_scopes(active_color);
            update_targets_and_moves(active_color);

        }

        void record_move (string origin_coord_str, string target_coord_str, char origin_symbol, int origin_color, char target_symbol) {

            /* Records a move in PGN notation and appends it to history. */

            string move_notation = move_to_pgn(origin_coord_str, target_coord_str, origin_symbol, origin_color, target_symbol);
            if (origin_color == pieces.w)
                pgn_history[full_moves] = {};
            pgn_history[full_moves].push_back(move_notation);

        }

        snapshot record_snapshot () {
            /* Records and returns snapshot. */
            snapshot snap;
            snap.color = active_color;
            snap.full_moves = full_moves;
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

        void revert (snapshot snap) {
            /* Reverts board to snapshot. */
            active_color = snap.color;
            full_moves = snap.full_moves;
            pgn_history = snap.pgn_history;
            en_passant_coord = snap.en_passant_coord;
            white_is_checked = snap.white_is_checked;
            black_is_checked = snap.black_is_checked;
            all_castling_rights_lost = snap.all_castling_rights_lost;
            castling_right_k_w = snap.castling_right_k_w;
            castling_right_q_w = snap.castling_right_q_w;
            castling_right_k_b = snap.castling_right_k_b;
            castling_right_q_b = snap.castling_right_q_b;
            white_king_coord = snap.white_king_coord;
            black_king_coord = snap.black_king_coord;
            white_symbol_occupation_map = snap.white_occupation_map;
            black_symbol_occupation_map = snap.black_occupation_map;
            white_targets = snap.white_targets;
            black_targets = snap.black_targets;
            white_moves = snap.white_moves;
            black_moves = snap.black_moves;
            white_pins = snap.white_pins;
            black_pins = snap.black_pins;
            white_spaces = snap.white_spaces;
            black_spaces = snap.black_spaces;
        }

        void tap_clock () {

            /* Adjusts the game clock by incrementing half clock and full moves accordingly. */

            half_clock++;
            if (half_clock > 2 * full_moves)
                full_moves++;

        }


        // chess rules and logic
        bool can_castle_king_side (int color) {
            
            /* Checks if a color can castle king side. */

            if (all_castling_rights_lost)

                return 0;

            else if (color == pieces.w && !white_is_checked || color == pieces.b && !black_is_checked) {

                if (color == pieces.w && castling_right_k_w) {

                    if (!square_is_occupied("F1") && 
                        !square_is_occupied("G1") && 
                        !square_is_targeted(color, "F1") && 
                        !square_is_targeted(color, "G1"))
                        return 1;
                    
                } else if (color == pieces.b && castling_right_k_b) {

                    if (!square_is_occupied("F8") && 
                        !square_is_occupied("G8") && 
                        !square_is_targeted(color, "F8") && 
                        !square_is_targeted(color, "G8"))
                        return 1;

                }

            } 
            
            return 0;

        };

        bool can_castle_queen_side (int color) {

            /* Checks if a color can castle queen side. */

            if (all_castling_rights_lost)

                return 0;

            else if (color == pieces.w && !white_is_checked || color == pieces.b && !black_is_checked) {

                if (color == pieces.w && castling_right_q_w) {

                    if (!square_is_occupied("C1") && 
                        !square_is_occupied("D1") && 
                        !square_is_targeted(color, "C1") && 
                        !square_is_targeted(color, "D1"))
                        return 1;
                    
                } else if (color == pieces.b && castling_right_q_b) {

                    if (!square_is_occupied("C8") && 
                        !square_is_occupied("D8") && 
                        !square_is_targeted(color, "C8") && 
                        !square_is_targeted(color, "D8"))
                        return 1;

                }

            } 
            
            return 0;

        };

        bool inside_bounds (int rank, int file) {

            /* Checks if the rank and file integer provided map to a valid chess square. */

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
                            if (!square_is_occupied_by_color(color, target_coord))
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
                                        // int attacker_color;
                                        // if (color == pieces.w)
                                        //     attacker_color = pieces.b;
                                        // else
                                        //     attacker_color = pieces.w;

                                        // only consider the target square if its not protected
                                        if (!square_is_targeted(color, target_coord)) {
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

        bool square_is_targeted (int color, string coord_str) {
            /* Checks if a square is targeted by opposing color.
            Provided color is the friendly color. */
            map<string, vector<string>> targets;
            if (color == pieces.w)
                targets = black_targets;
            else
                targets = white_targets;
            // iterate through targets
            for (auto const& x : targets) {
                if (contains_string(x.second, coord_str))
                    return 1;
            }
            return 0;

        }

        void update_king_scopes (int king_color) {
            
            /* Updates king scope variables.
            This method should be called in refresh method within an active move which was played.
            The algorithm will locate the new active king from which it will screen all directions
            for attackers, checks and denote pinned coordinates and space to a checking attacker
            i.e. a vector of empty in-between squares. */
            
            // declarevariables
            vector<std::vector<int>> offs; // 2D offset vector
            int file, 
                rank, 
                f, 
                r,
                pointer_piece,
                pointer_color,
                forward_rank,
                curr_file;
            vector<int> fr;
            vector<string> spacing;
            string king_coord, 
                   first_friendly_coord,
                   pointer;
            bool is_checked = 0,
                 was_checked_in_this_iteration = 0;
            char pointer_symbol,
                 pawn_symbol;

            // reset global vectors for direct insertion, and get color
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

            // derive file and rank
            fr = get_file_and_rank_from_coord(king_coord);
            file = fr[0];
            rank = fr[1];

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
                        f = file + step * offs[ind][0];
                        r = rank + step * offs[ind][1];
                        
                        // break offset direction search if exceeds the board
                        if (!inside_bounds(r, f))
                            break;

                        // compute pointer info
                        pointer = get_coord_from_file_and_rank(r, f);
                        pointer_symbol = get_symbol_from_coord(pointer);
                        pointer_piece = pieces.from_symbol(pointer_symbol);

                        // break direction if a pawn is hit, as they will be calculated seperately
                        if (pointer_piece == pieces.Pawn)
                            break;
                        
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
                if (pointer_piece == pieces.Knight && 
                    get_color_from_symbol(pointer_symbol) != king_color) {
                    
                    // denote check
                    is_checked = 1;
                    check_coords.push_back(pointer);

                    // only one knight can check at a time, so break
                    break;

                }

            }
            
            // finally probe if checked by pawn
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
            if (is_checked) {
                if (king_color == pieces.w)
                    white_is_checked = 1;
                else
                    black_is_checked = 1;
            } else {
                white_is_checked = black_is_checked = 0;
            }

        }

        void update_targets_and_moves (int color) {

            /* Integrated and improved method which updates 
            targets and legal moves for a color. 
            Should be called after king scope was updated. */

            // init new empty dummy maps and variables
            map<string, vector<string>> target_map, 
                                        move_map, 
                                        pins, 
                                        spaces;
            map<string, char> occupation_map = get_occupation_map(color);
            string coord;
            char symbol;
            vector<string> intersection, 
                           targets, 
                           moves, 
                           pre_targets;
            
            // first determine the targets for active color anyway
            // as those will be crucial for the next move (i.e. enemy) 
            for (auto const& x : occupation_map) {

                coord = x.first;
                symbol = x.second;

                // denote target coordinates
                targets.clear();
                pre_targets = reachable_target_coords(coord, symbol, color);

                // first remove the non-legal pawn capture moves.
                // check if the diagonal pawn targets really hold enemies
                if (pieces.from_symbol(symbol) == pieces.Pawn) {

                    const char pawn_file = coord[0];
                    if (color == pieces.w)
                        pins = white_pins;
                    else
                        pins = black_pins;
                        
                    for (int i = 0; i < pre_targets.size(); i++) {
                        // add target to moves if it is same file
                        // in this case that is not a target but only a move
                        // as the pawn cannot capture by pushing forward
                        if (pawn_file == pre_targets[i][0]) {
                            if (!move_map.count(coord)) 
                                move_map[coord] = {};
                            move_map[coord].push_back(pre_targets[i]); 
                        }
                        // if pawn is on another file check for enemies to be a valid move
                        // otherwise check if the target is an en-passant target
                        else if (square_is_occupied_by_enemy(color, pre_targets[i]) ||
                                 en_passant_coord != "-" && pre_targets[i] == en_passant_coord) {
                            move_map[coord].push_back(pre_targets[i]);
                            targets.push_back(pre_targets[i]);
                        } 
                        // empty diagonal target is a real target square
                        else {
                            targets.push_back(pre_targets[i]);
                        }
                    }

                } else {
                    // otherwise all other pieces can directly hold their targets
                    // as all can potentially turn into moves.
                    targets = pre_targets;
                }

                // override target map
                target_map[coord] = targets;

            }
            
            // check if provided color is in check
            if (color == pieces.w && white_is_checked || color == pieces.b && black_is_checked) {
                
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
                        
                        // skip pawns as they were added already
                        if (pieces.from_symbol(x.second) == pieces.Pawn)
                            continue;
                        
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
                
                // select the right pins
                if (color == pieces.w)
                    pins = white_pins;
                else
                    pins = black_pins;
                
                // check if the yet added pawns arent pinned,
                // if the pawn is pinned restrict it's moves further [bug: alloc error]
                for (auto const& y : move_map) {
                    // print("0", "test");
                    // print(y.first, "test");
                    coord = y.first;
                    moves = y.second;
                    // print("1", "test");
                    // check if the pawn is pinned
                    if (pins.count(coord)) {
                        // select which moves are within the pins
                        intersection = intersect(moves, pins[coord]);
                        if (intersection.size())
                            move_map[coord] = intersection;
                        else
                            move_map.erase(coord);
                        continue;
                    }
                        
                    // print("2", "test");
                    // // otherwise check if it can block the check
                    // intersection = intersect(moves, spaces[0]);
                    // print("3", "test");
                    // if (intersection.size())
                    //     move_map[coord] = intersection;
                    // else
                    //     // again this pawn has no moves, so erase from move_map
                    //     move_map.erase(coord);
                    // print("4", "test");
                }
                
                vector<string> space;
                for (auto const& x : occupation_map) {

                    coord = x.first;
                    symbol = x.second;

                    // skip pawns as they are solved already
                    if (symbol == 'p' || symbol == 'P')
                        continue;

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
                    
                    // append castle moves if symbol is a king
                    if (!all_castling_rights_lost) {
                        if (symbol == 'K') {
                            if (can_castle_king_side(pieces.w))
                                move_map[white_king_coord].push_back("G1");
                            if (can_castle_queen_side(pieces.w))
                                move_map[white_king_coord].push_back("C1");
                        } else if (symbol == 'k') {
                            if (can_castle_king_side(pieces.b))
                                move_map[black_king_coord].push_back("G8");
                            if (can_castle_queen_side(pieces.b))
                                move_map[black_king_coord].push_back("C8");
                        }
                    }

                }

            }
            
            // override global maps
            if (color == pieces.w) {
                white_targets = target_map;
                white_moves = move_map;
            } else {
                black_targets = target_map;
                black_moves = move_map;
            }

        }

};