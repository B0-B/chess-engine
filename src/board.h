#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <regex>

// include objects and libraries
#include "piece.h"
#include "general.h"
#include "snapshot.h"

using namespace std;
using std::filesystem::current_path;


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

        string __path__;

        // ---- new method set ----

        // Load the pieces
        Piece pieces;

        int active_color = pieces.w;

        // prepare letters for the board files
        map<char, int> file_id = { {'A', 0}, {'B', 1}, {'C', 2}, {'D', 3}, {'E', 4}, {'F', 5}, {'G', 6}, {'H', 7} }; // quick map
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

        // board pointer variables, scope and target context
        // these variables are dynamically used during scope and move search
        int file, 
            rank, 
            f, 
            r,
            pointer_piece,
            pointer_color,
            forward_rank,
            curr_file;
        vector<std::vector<int>> offs; // 2D offset selector
        vector<int> fr;
        vector<string> spacing,
                       check_screening_squares; // squares behind the king which are on LoS of scoper during a check
        string king_coord, 
               first_friendly_coord,
               king_target,
               coord,
               pointer;
        bool is_checked = 0,
             was_checked_in_this_iteration = 0;
        char pointer_symbol,
             pawn_symbol,
             symbol,
             king_symbol;
        map<string, vector<string>> target_map, 
                                    move_map, 
                                    pins, 
                                    spaces,
                                    enemys_targets;
        map<string, char> occupation_map;
        vector<string> intersection, 
                       targets, 
                       moves, 
                       pre_targets,
                       king_targets,
                       escape_coords;


        // constructor sequence
        Board(void) {

            console("Initialize ...", "board");

            // determine current parent directory from here
            __path__ = current_path().u8string();
            if (!contains_substring(__path__, "src"))
                #ifdef _WIN32
                    __path__ += "\\src\\";
                #elif __linux__
                    __path__ += "/src/";
                #endif
            
            // load board ascii graphics
            load_board_ascii();

            // reserve space for vectors
            reserve_pointers();

            cout << "Current working directory: " << __path__ << endl;

        }


        // stdout methods
        void show_board () {

            /* Shows the current board state in the console. 
            The output is build from the board ascii template. */

            int ind = 0;
            string coord,
                   board_ascii = board_ascii_template,
                   symbol,
                   square_number;

            // fill all 64 squares
            for (int r = 0; r < 8; r++) {   

                for (int f = 0; f < 8; f++) {
                    
                    // convert square id to coordinate
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
                    
                    // override the board_ascii string by overriding the square number occurence with symbol
                    board_ascii = regex_replace(board_ascii, regex(square_number), symbol);
                
                }

            }

            print(board_ascii);

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

            console("moves for " + pieces.color_string(color), "board");
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
            console("Occupation map for " + pieces.color_string(color), "board");
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

            console("targets for " + pieces.color_string(color), "board");
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


        // get and conversion methods
        int get_color_from_symbol (char symbol) {

            /* Returns the piece color int from symbol char e.g. k -> 16, Q -> 8
            where 16 is black and 8 white. If the symbol is None '_' the return will be 0. */

            // __call__(__func__);
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

        tuple<int, int> get_file_and_rank_from_coord (string coord_str) {

            /* Returns a integer tuple with file and rank (between 0 and 7). */

            // quick map from file character to file integer
            f = file_id[coord_str[0]];

            // convert rank
            int r = coord_str[1] - '0';
            r--;

            return make_tuple(f, r);

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
                return '_';
        };
        

        // board initialization
        void load_board_ascii () {

            /* Loads the ascii board template from file to string variable. */

            // string ascii_path = current_path();
            std::ifstream inFile;
            string file_path = __path__ + "board_ascii_template.txt";
            inFile.open(file_path); //open the input file

            std::stringstream strStream;
            strStream << inFile.rdbuf(); //read the file
            std::string str = strStream.str(); //str holds the content of the file

            board_ascii_template = str;

        }

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
                        file += _char - '0'; // - 1;
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

                    en_passant_coord += toupper(_char);
                    if (_char == '-' || en_passant_coord.size() >= 2) {
                        en_passant_parsed = 1;
                        i++;
                    }

                // parse and denote half clock count
                } else if (en_passant_parsed && !half_clock_parsed) {

                    if (_char == ' ')
                        half_clock_parsed = 1;
                    else
                        half_clock += _char;
                    i++;

                // parse and denote the move count
                } else if (half_clock_parsed && !full_moves_parsed) {

                    moves += _char;
                    if (i == fen.size()-1) {
                        full_moves = stoi(moves);
                        if (verbose)
                            console("Successfully loaded position from FEN.", "Board");
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

            if (verbose)
                cout << "load starting position ..." << endl;

            // load position from FEN code    
            load_position_from_fen(starting_position_fen, verbose);

            if (verbose)
                cout << "successfully loaded starting position." << endl;

        };

        void reserve_pointers () {

            /* Reserves the needed vector spaces for 
            global and dynamic variables. */

            check_coords.reserve(2);
            black_scopers.reserve(5);
            white_scopers.reserve(5);

            // pointer variables
            offs.reserve(8);
            fr.reserve(2);
            spacing.reserve(6);
            intersection.reserve(6);
            targets.reserve(28);
            pre_targets.reserve(28);
            moves.reserve(28);
            king_targets.reserve(8);
            escape_coords.reserve(7);

        }


        // board operations
        void flip_active_color () {
            /* Flips the active playing color. */
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
            auto [origin_file, origin_rank] = get_file_and_rank_from_coord(origin_coord_str);
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
                        auto [attacker_file, attacker_rank] = get_file_and_rank_from_coord(origin_coord_str);
                        attacker_piece = pieces.from_symbol(attacker_occupation_map[attacker_coord]);
                        
                        // if the attacking piece is no pawn and is one of two pieces of same kind which attack add the file
                        if (attacker_piece == origin_piece) {
                            
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
            const char symbol = get_symbol_from_coord(origin_coord_str);
            const int color = get_color_from_symbol(symbol);
            const map<string, char> occupation_map = get_occupation_map(color);
            const snapshot snap = record_snapshot();
            map<string, vector<string>> targets = get_targets(color);
            map<string, vector<string>> moves = get_moves(color); 
            const char captured_symbol = get_symbol_from_coord(target_coord_str); // denote captured symbol for later notation

            // check if the move is legal by probing all illegal conditions, 
            // except when it's forced, then skip
            if (!force) {
                // wrong color is playing
                if (color != active_color) {
                    console("It's not " + pieces.color_string(color) + "'s turn.", "board");
                    return snap;
                }
                // origin coordinate does not 
                if (occupation_map.count(origin_coord_str) == 0) {
                    console(origin_coord_str + "holds no " + pieces.color_string(color) + " pieces.", "board");
                    return snap;
                }
                // move not contained in allowed moves
                if (!contains_string(moves[origin_coord_str], target_coord_str)) {
                    console(pieces.color_string(color) + "'s move " + origin_coord_str + "->" + target_coord_str + " is not legal.", "board");
                    show_board();
                    return snap;
                }
                // target square is occupied by a friendly piece
                if (color == get_color_from_symbol(captured_symbol)) {
                    cout << "test: " << color << " " << get_color_from_symbol(captured_symbol) << " " << captured_symbol << endl;
                    console(target_coord_str + " is occupied by friendly piece.", "board"); 
                    return snap;
                
                }
            }

            // save info to snapshot object
            // const snapshot snap = record_snapshot();

            // take action on board
            remove_symbol_at_coord(symbol, origin_coord_str);
            place_symbol_at_coord(symbol, target_coord_str);

            // piece specific cases
            const int piece = pieces.from_symbol(symbol);

            /* ---- appendix moves ---- */

            // check if pawn move leaves an en-passant possibility
            if (piece == pieces.Pawn) {

                const auto [_, origin_rank] = get_file_and_rank_from_coord(origin_coord_str);
                const auto [target_file, target_rank] = get_file_and_rank_from_coord(target_coord_str);

                // 1) check if en-passant is captured and remove the captured pawn
                if (target_coord_str == en_passant_coord) {
                    const auto [en_passant_file, en_passant_rank] = get_file_and_rank_from_coord(en_passant_coord);
                    string capture_coord;
                    char captured_pawn_symbol;
                    if (color == pieces.w)
                        captured_pawn_symbol = 'p';
                    else
                        captured_pawn_symbol = 'P';
                    if (en_passant_rank == 2) 
                        capture_coord = get_coord_from_file_and_rank(en_passant_file, en_passant_rank + 1);
                    else
                        capture_coord = get_coord_from_file_and_rank(en_passant_file, en_passant_rank - 1);
                    remove_symbol_at_coord(captured_pawn_symbol, capture_coord);
                }
                
                // 2) finally check if the pawn is moved from origin square for two ranks at once
                if ((active_color == pieces.w && origin_rank == 1 && target_rank == 3) || 
                    (active_color == pieces.b && origin_rank == 6 && target_rank == 4)) {
                    
                    char symbol;

                    // check if a new en-passant possibility pops up for enemy
                    int sign = 1, 
                        neighbour_file,
                        en_passant_target_rank;
                    string neighbour_coord, 
                           en_passant_target_rank_str;
                    
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
                    remove_symbol_at_coord('r', "H8");
                    place_symbol_at_coord('r', "F8");
                    // remove all castling rights for white
                    castling_right_k_b = castling_right_q_b = 0;

                } else if (color == pieces.b && origin_coord_str == "E8" && target_coord_str == "C8") {

                    // first check if castling is allowed
                    remove_symbol_at_coord('r', "A8");
                    place_symbol_at_coord('r', "D8");
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
            if (!all_castling_rights_lost) {
                // check if castling rights might be lost due to a king or rook move
                if (piece == pieces.King || piece == pieces.Rook && 
                    castling_right_k_w + castling_right_q_w + castling_right_k_b + castling_right_q_b == 0)
                        all_castling_rights_lost = 1;
                // also check if a rook is captured to remove the castling rights on the corresponding side
                if (captured_symbol == 'R' || captured_symbol == 'r') {
                    if (target_coord_str == "A1")
                        castling_right_q_w = 0;
                    else if (target_coord_str == "A8")
                        castling_right_k_w = 0;
                    else if (target_coord_str == "H1")
                        castling_right_q_b = 0;
                    else if (target_coord_str == "H8")
                        castling_right_k_b = 0;
                }
            }

            // denote the move and add to history
            // except when a move is forced (for performance reasons)
            if (!force)
                record_move(origin_coord_str, target_coord_str, symbol, color, captured_symbol);

            // update move counts (only if not forced)
            if (!force)
                tap_clock();
                
            flip_active_color();

            // update the king scopes // only after 2nd half move, because checks and pins aren't possibe (dismissed)
            update_king_scopes(active_color);

            // finally update targets
            update_targets_and_moves(active_color);

            // check if a mate was delivered
            // i.e. if the active color doesn't have any moves left.
            // code here ...
            if (true) {
                if (get_moves(active_color).empty()) {
                    console(pieces.color_string(active_color) + " got checkmated!", "board");
                }
            }

            return snap;

        }

        void record_move (string origin_coord_str, string target_coord_str, char origin_symbol, int origin_color, char target_symbol) {

            /* Records a move in PGN notation and appends it to history. */

            if (origin_color == pieces.w)
                pgn_history[full_moves] = {};
            pgn_history[full_moves].push_back(
                move_to_pgn(origin_coord_str, target_coord_str, origin_symbol, origin_color, target_symbol)
            );

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
            
            /* Checks if a color can castle king side.  
            This method assumes that there is no check which would prevent this. */

            // shortcut if there is no castling rights
            if (all_castling_rights_lost)
                return 0;

            // it was checked already if in check or not
            // if in check this method will not be called
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
            
            return 0;

        };

        bool can_castle_queen_side (int color) {

            /* Checks if a color can castle queen side. */

            if (all_castling_rights_lost)
                return 0;

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
            
            return 0;

        };

        bool inside_bounds (int file, int rank) {

            /* Checks if the rank and file integer provided map to a valid chess square. */

            // simply check if the indices lie within the boundaries
            return rank > -1 && rank < 8 && file > -1 && file < 8;
        }

        vector<string> reachable_target_coords (string coord_str, char symbol, int color) {

            /* 
            Returns a string vector of reachable target coordinates.
            */
            
            string target_coord;
            vector<string> out = {};
            vector<vector<int>> decrement;
            vector<char> screeningDirections;

            // int color = get_color_from_symbol(symbol);
            const int piece = pieces.from_symbol(symbol);

            // extract file and rank from origin coord
            auto [file, rank] = get_file_and_rank_from_coord(coord_str);

            // pick the screening directions depending on piece
            if (piece == pieces.Pawn) 
                screeningDirections.push_back('p');
            else if (piece == pieces.Knight)
                screeningDirections.push_back('n');
            else if (piece == pieces.Bishop)
                screeningDirections.push_back('d');
            else if (piece == pieces.Rook)
                screeningDirections = {'v', 'h'};
            // otherwise must be queen or king
            else
                screeningDirections = {'v', 'h', 'd'};
                
            // iterate through selected cases
            for (int i = 0; i < screeningDirections.size(); i++) {
                
                if (screeningDirections[i] == 'p') { // edge case for pawn

                    int sign=1, steps=1;
                    if (color == pieces.b) 
                        sign = -1;
                    if ((sign == 1 && rank == 1) || (sign == -1 && rank == 6)) 
                        steps = 2;

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

                        if (inside_bounds(file+(2*i-1), rank+sign)) {
                            
                            // target_coord = get_coord_from_file_and_rank(file+pow(-1.,i), rank+sign);
                            target_coord = get_coord_from_file_and_rank(file+(2*i-1), rank+sign);

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
                        print("TEST: " + en_passant_coord);
                        // check if the selected pawn is next to en-passant coord
                        auto [_file, _rank] = get_file_and_rank_from_coord(en_passant_coord);
                        // (file+1 == _file || file-1 == _file)
                        cout << "TEST2 " << _file << " " << _rank << " " << file << " " << rank;
                        if (rank == _rank-sign && abs(file - _file) == 1)
                            out.push_back(en_passant_coord);
                    }

                    continue;
                    
                } else if (screeningDirections[i] == 'n') { // edge case for knight
                    int r, f;
                    for (int i = 0; i < 8; i++) {
                        r = rank + n_offset[i][0], 
                        f = file + n_offset[i][1];
                        if (inside_bounds(r, f)) {
                            target_coord = get_coord_from_file_and_rank(f, r);
                            // if (!square_is_occupied(target_coord) || square_is_occupied_by_enemy(color, target_coord))
                            // improved: instead ask if square is not occupied by a friendly piece -> only one operation
                            if (!square_is_occupied_by_color(color, target_coord))
                                out.push_back(target_coord);
                        }
                    }
                    continue;
                } else if (screeningDirections[i] == 'd')   // load diagonal decrementation coords
                    decrement = d_offset;
                else if (screeningDirections[i] == 'v')   // load vertical decrementation coords
                    decrement = v_offset;
                else if (screeningDirections[i] == 'h')   // load horizontal decrementation coords
                    decrement = h_offset;

                // decrementation algorithm for v, h, d
                // code continues if piece is not a pawn or knight
                // start iterate into scoper directions
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

                        // break scoping if inside bounds
                        if (!inside_bounds(r, f))
                            break;
                        
                        // otherwise continue 
                        target_coord = get_coord_from_file_and_rank(f, r);

                        // break if a piece is hit
                        if (square_is_occupied(target_coord)) {
                            // add target if enemy piece is at the end
                            if (square_is_occupied_by_enemy(color, target_coord))
                                out.push_back(target_coord);
                            break;
                        }

                        // otherwise the target square is empty - append
                        out.push_back(target_coord);
                        
                    }
                    
                }
                
            }
            
            // this is wrong, as castling square is not a scoping square
            // check for castling opportunity in case of king
            // if (piece == pieces.King) {
            //     if (can_castle_king_side(color)) {
            //         if (color == pieces.w) 
            //             out.push_back("G1");
            //         else
            //             out.push_back("G8");
            //     }
            //     if (can_castle_queen_side(color)) {
            //         if (color == pieces.w) 
            //             out.push_back("C1");
            //         else
            //             out.push_back("C8");
            //     }
            // }

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
                return square_is_occupied_by_color (pieces.b, coord_str);
            else
                return square_is_occupied_by_color (pieces.w, coord_str);
        }

        bool square_is_targeted (int color, string coord_str) {

            /* Checks if a square is targeted by opposing color.
            Provided color is the friendly color. */
            
            // derive file and rank
            auto [file, rank] = get_file_and_rank_from_coord(coord_str);

            // check if coord is attacked by scopers, i.e. Q,K,B,R or pawns
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

                    for (int step = 1; step < 7; step++) {
                        
                        // shift pointer by one offset step
                        f = file + step * offs[ind][0];
                        r = rank + step * offs[ind][1];
                        
                        // break offset direction search if exceeds the board
                        if (!inside_bounds(f, r))
                            break;

                        // compute pointer info
                        pointer = get_coord_from_file_and_rank(f, r);
                        pointer_symbol = get_symbol_from_coord(pointer);
                        pointer_color = get_color_from_symbol(pointer_symbol);
                        pointer_piece = pieces.from_symbol(pointer_symbol);        

                        // check if pointed pawn is checking, in that case brake directional search
                        if (step == 1 && d == 2 &&
                            pointer_piece == pieces.Pawn && 
                            pointer_color != color && 
                            (color == pieces.w && rank + 1 == r || color == pieces.b && rank - 1 == r) )
                            return 1;
                                                
                        
                        // skip empty pieces, but denote in spacing vector
                        if (pointer_piece == pieces.None)
                            continue;

                        // is occupied by enemy
                        if (pointer_color != color) {
                            
                            // check if the offset direction matches the possible attacker pieces
                            // e.g. vertical and horizontal: Rook and Queen
                            if ( ((d == 0 || d == 1) && (pointer_piece == pieces.Rook || pointer_piece == pieces.Queen)) ||
                                 ((d == 2) && (pointer_piece == pieces.Bishop || pointer_piece == pieces.Queen)) )
                                return 1;

                        }

                    }

                }

            }

            // check if knight is targeting
            for (int k = 0; k < n_offset.size(); k++) {
                
                // shift pointer by one offset step
                f = rank + n_offset[k][0];
                r = file + n_offset[k][1];
                
                // break offset direction search if exceeds the board
                if (!inside_bounds(r, f))
                    continue;

                // compute pointer info
                pointer = get_coord_from_file_and_rank(r, f);
                pointer_symbol = get_symbol_from_coord(pointer);
                pointer_piece = pieces.from_symbol(pointer_symbol);
                pointer_color = get_color_from_symbol(pointer_symbol);
                
                // verify it's an opposing knight
                if (pointer_piece == pieces.Knight && 
                    pointer_color != color) 
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
            

            // reset dynamic variables
            first_friendly_coord = "",
            is_checked = 0,
            was_checked_in_this_iteration = 0;
            check_screening_squares.clear();
            check_coords.clear();

            // reset global vectors for direct insertion, and get color
            if (king_color == pieces.w) {
                king_coord = white_king_coord;
                white_pins.clear();
                white_spaces.clear();
                black_scopers.clear();
            } else {
                king_coord = black_king_coord;
                black_pins.clear();
                black_spaces.clear();
                white_scopers.clear();
            }

            // derive file and rank
            auto [file, rank] = get_file_and_rank_from_coord(king_coord);

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
                        if (!inside_bounds(f, r))
                            break;

                        // compute pointer info
                        pointer = get_coord_from_file_and_rank(f, r);
                        pointer_symbol = get_symbol_from_coord(pointer);
                        pointer_color = get_color_from_symbol(pointer_symbol);
                        pointer_piece = pieces.from_symbol(pointer_symbol);

                        // check if pointed pawn is checking, in that case brake directional search
                        if (step == 1 &&
                            pointer_piece == pieces.Pawn && 
                            pointer_color != king_color && 
                            d == 2 ) 
                        {
                                if (king_color == pieces.w && rank + 1 == r || 
                                    king_color == pieces.b && rank - 1 == r)   
                                {
                                        check_coords.push_back(pointer);
                                        is_checked = 1;
                                        break;
                                }
                        }
                            
                        
                        // skip empty pieces, but denote in spacing vector
                        if (pointer_piece == pieces.None) {
                            spacing.push_back(pointer);
                            continue;
                        }
                        

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

                                    // revert back for screening squares behind the king
                                    string probe_coord;
                                    for (int step = 1; step < 7; step++) {
                        
                                        // shift pointer by one offset step
                                        f = file - step * offs[ind][0];
                                        r = rank -  step * offs[ind][1];

                                        // check if valid screening square, i.e. eiter occupied by enemy or empty
                                        if ( !inside_bounds(f, r) )
                                            break;

                                        probe_coord = get_coord_from_file_and_rank(f, r);
                                        
                                        // otherwise break if a friendly piece is behind the king as the king 
                                        // cannot escape there anyway
                                        if (square_is_occupied_by_color(king_color, probe_coord))
                                            break; 

                                        // here th quare is either empty or occupied by enemy
                                        // add it to screening squares
                                        check_screening_squares.push_back(probe_coord);

                                        // again break if the screen is occupied at all
                                        if (square_is_occupied_by_enemy(king_color, probe_coord))
                                            break;

                                    }
                                }

                                // remember spacing squares between king and attacker globally,
                                // or king and pinned piece and pinned piece and attacker
                                if (step > 1 && was_checked_in_this_iteration) {
                                    if (king_color == pieces.w)
                                        white_spaces[pointer] = spacing; // ?
                                    else
                                        black_spaces[pointer] = spacing;
                                }
                                
                                

                            } 

                            // end here since an enemy piece was detected
                            break;

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
                f = rank + n_offset[k][0];
                r = file + n_offset[k][1];
                
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
            // map<string, vector<string>> target_map, 
            //                             move_map, 
            //                             pins, 
            //                             spaces;
            // map<string, char> occupation_map = get_occupation_map(color);
            // string coord;
            // char symbol;
            // vector<string> intersection, 
            //                targets, 
            //                moves, 
            //                pre_targets;

            map<string, char> occupation_map = get_occupation_map(color);

            // reset objects
            intersection.clear();
            targets.clear(); 
            moves.clear(); 
            pre_targets.clear();
            target_map.clear();
            move_map.clear();
            pins.clear();
            spaces.clear();
            
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
                
                // get king coord, current pins, spaces and color
                if (color == pieces.w) {
                    king_coord = white_king_coord;
                    enemys_targets = black_targets;
                    pins = white_pins;
                    spaces = white_spaces;
                } else {
                    king_coord = black_king_coord;
                    enemys_targets = white_targets;
                    pins = black_pins;
                    spaces = black_spaces;
                }
                
                // and king symbol from occupation mapping
                king_symbol = occupation_map[king_coord];
                
                // draw targets for the attacked king, 
                // the targets were already computed
                // determine king escape squares
                escape_coords.clear();
                for (int i = 0; i < target_map[king_coord].size(); i++) {

                    // target_is_attacked=0;
                    king_target = target_map[king_coord][i];

                    // skip if occupied by friendly piece
                    if (square_is_occupied_by_color(color, king_target))
                        continue;
                    
                    // skip the square if it's screened by the scoper during check
                    if (contains_string(check_screening_squares, king_target))
                        continue;
                    
                    // otherwise check if the escape square is protected
                    // this holds for empty squares and enemy-occupied squares
                    bool target_in_spaces;
                    for (auto const& x : spaces) {
                        if (contains_string(x.second, king_target)) {
                            target_in_spaces = 1;
                            break;
                        }
                    }
                    if (target_in_spaces)
                        continue;
                    
                    // otherwise this escape square is valid
                    escape_coords.push_back(king_target);

                }

                // then check the number of checkers, if greater than 1 the king has to move
                // and we can neglect all other pieces for performance
                if (check_coords.size() > 1) {

                    // delete the move map as only the king will be able to move to escape 
                    move_map.clear();

                    // add the king escape coords only to valid move map
                    move_map[king_coord] = escape_coords;
                    
                }

                // otherwise the king is attacked only once and other friendly pieces may interfere the check
                else {
                    
                    // if there is a check we need to revert to the yet added pawn moves and restrict them
                    for (auto const& x : move_map) {

                        coord = x.first;
                        moves = x.second;
                        
                        // skip if the pawn is pinned
                        if (pins.count(coord)) {
                            // this pawn has no moves, so erase from move_map
                            move_map.erase(coord);
                            continue;
                        }

                        // otherwise check if it can block the check
                        intersection = intersect(moves, spaces[check_coords[0]]);
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
                    int p;
                    for (auto const& x : occupation_map) {
                        
                        coord = x.first;
                        p = pieces.from_symbol(x.second);

                        // skip king and pawns
                        if (p == pieces.King || p == pieces.Pawn)
                            continue;
                        
                        // skip if the piece is pinned as it cannot
                        // come to help in that case
                        if (pins.count(coord))
                            continue;

                        symbol = x.second;
                        // ???
                        // vector<string> targets = reachable_target_coords(coord, symbol, color);
                        targets = target_map[coord]; 
                    
                        // denote legal moves if the piece can block the check
                        // i.e. intersect the line between scoper and king
                        if (spaces.count(check_coords[0])) {
                            // if (p == pieces.Pawn)
                            //     intersection = intersect(move_map[coord], spaces[check_coords[0]]);
                            // else
                            intersection = intersect(targets, spaces[check_coords[0]]);
                            if (intersection.size())
                                move_map[coord] = intersection;
                        }

                        // check if the non-pinned piece can capture the only checking scoper
                        // if (p == pieces.Pawn && contains_string(move_map[coord], check_coords[0])) {
                        //     move_map[coord].push_back(check_coords[0]);
                        // } else 
                        
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

                    coord = y.first;
                    moves = y.second;
                    
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

                    // if symbol is king restrict which targets are not protected
                    // this is already fulfilled in reachable_target_coords
                    if ( 0 && ( symbol == 'k' || symbol == 'K' ) ) {
                        // if (move_map.count(coord))
                        //     move_map[coord].clear();
                        // else
                        //     move_map[coord] = {};
                        for (int i = 0; i < targets.size(); i++) 
                            if ( !square_is_targeted(color, targets[i]) )
                                move_map[coord].push_back(targets[i]);
                        if (!move_map[coord].size())
                            move_map.erase(coord);
                        continue;
                    }

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

                        // only override the move map if there are any valid moves left
                        if (intersection.size())
                            move_map[coord] = intersection;

                        // also check if the scoper can be captured by the pinned piece
                        // not sure if this will work, as another scoper could evtl. be captured
                        if (color == pieces.w) {
                            intersection = intersect(targets, black_scopers); // ?
                        } else {
                            intersection = intersect(targets, white_scopers);
                        }

                        // if the scoper is included, add it's coord to move map
                        if (intersection.size()) {
                            if (move_map.count(coord))
                                move_map[coord].clear();
                            move_map[coord].push_back(intersection[0]);
                        }

                    }

                    // otherwise append targets as valid moves
                    else
                        move_map[coord] = targets;

                }

                // since color is not checked, append castle moves if privileges exist
                if (!all_castling_rights_lost) {
                    if (color == pieces.w) {
                        if (can_castle_king_side(pieces.w))
                            move_map[white_king_coord].push_back("G1");
                        if (can_castle_queen_side(pieces.w))
                            move_map[white_king_coord].push_back("C1");
                    } else {
                        if (can_castle_king_side(pieces.b))
                            move_map[black_king_coord].push_back("G8");
                        if (can_castle_queen_side(pieces.b))
                            move_map[black_king_coord].push_back("C8");
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