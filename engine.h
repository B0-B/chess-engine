/*  Chess Engine Implementation */

#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <chrono>

// include objects
#include "board.h" // board comes with pieces

class Engine {

    public:

        Board board_main;
        Board board_test;

        Engine(void) {
            cout << "load engine ..." << endl;
        };

        /* test suites and simulations */
        void shannon_benchmark (int depth, bool visual=false) {
            
            /* An alias for shannon_number_simulation.
            Performs a time performance benchmark by stopping the time for each depth. */
            
            shannon_number_simulation (depth, visual);
            
        };

        void shannon_number_simulation (int depth, bool visual=false, bool clear=true) {
            
            /*
            Counts all possible move sequences i.e. the shannon number up to a specific depth.
            Sequence Count Table from start position:
            depth   lines
            1       20
            2       400
            3       8902
            4 ...
            */

            
            print("Start sequence count simulation test:", "Engine");
            // setup test board from start
            board_test.load_starting_position(0);
            board_test.refresh_position();
            int s;

            for (int d = 1; d <= depth; d++) {

                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                s = sequence_count_simulation(d, visual, clear);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                cout << "Sequences for depth " << d << " (" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1e6 << "s): " << s << endl;
            
            }
                
        }

    private:

        int sequence_count_simulation (int depth, bool visual=false, bool clear=true) {

            /* 
            An iterative sim approach to find the Shannon number for any specific depth. 
            This number of all possible positions does not account for transpositions of same pieces.
            The sim will start from current position.

            clear       Clear console after every board print.
                        Only used if visual is true.
            depth       Total analysis depth.
            visual      Show board after every move.

            */

            // check for depth arg
            if (depth == 0)
                return 1;

            // init counts of sequences
            int counts = 0;

            // draw the active move possibilities
            map<string, vector<string>> moves = board_test.get_possible_moves_for_active_color();
            
            string origin;
            vector<string> targets;
            MoveInfo move;

            // start
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now(); 
            print("0", "test");
            for (auto const& x : moves) {
                
                // extract keys and values i.e. origin coordinates 
                // and their corresponding target coordinates
                origin = x.first;
                targets = x.second;
                print("1", "test");
                for (int i = 0; i < targets.size(); i++) {

                    print("2", "test");
                    
                    // make an active move from origin to i-th target
                    move = board_test.active_move(origin, targets[i], 0);
                    print("3", "test");
                    if (visual) {
                        if (clear)
                            clear_console();
                        board_test.show_board();
                    }
                    
                    // repeat iteratively
                    counts += sequence_count_simulation(depth-1, visual);
                    print("4", "test");
                    board_test.active_undo_from_info(move);
                    print("5", "test");
                    // show board in console
                    if (visual) {
                        if (clear)
                            clear_console();
                        board_test.show_board();
                    }
                        
                }
            }
            print("6", "test");

            return counts;

        }
    
};