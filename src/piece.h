#include <stdio.h>
#include <iostream>
#include <string>

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
        const string unicode[13] = {" ", "♟", "♞", "♝", "♜", "♛", "♚", "♙", "♘", "♗", "♖", "♕", "♔"};

        int is_white (char symbol) {
            return symbol >= 'A' && symbol <= 'Z'; // ? the last condition is redundant
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

        float value (int piece) {

            /* Returns the historically known value of a piece. 
            The value is normalized by a pawn value i.e. 1. 
            Relative valuations were derived from alpha zero.
            https://en.wikipedia.org/wiki/Chess_piece_relative_value
            */

            if (piece == 1)
                return 1;
            else if (piece == 2)
                return 3.05;
            else if (piece == 3)
                return 3.33;
            else if (piece == 4)
                return 5.63;
            else if (piece == 5)
                return 9.5;
            return 0;
        };

        float value_from_symbol (char symbol) {

            /* Returns the historically known value of a piece. 
            The value is normalized by a pawn value i.e. 1. */
            
            return value(from_symbol(symbol));
            
        }

        string to_unicode (char symbol) {

            /* Returns corresponding unicode string. 
            e.g. K -> ♚ */

            int i;
            for (i = 0; i < symbols.size(); i++) {
                if (symbols[i] == symbol) {
                    break;
                }
            }
            return unicode[i];

        }

        string color_string (int color) {

            /* Returns the string name of current color. */

            if (color == 8)
                return "white";
            return "black";
            
        }
};
