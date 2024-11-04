/*  Chess Engine Implementation */

#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <chrono>

// include objects
#include "board.h" // board comes with pieces
// #include "snapshot.h"

class Engine {

    public:

        // init boards
        Board board_main;
        Board board_test;

        

        // evaluation
        float p;


        Engine(void) {
            cout << "load engine ..." << endl;
        };

        // ---- evaluation methods ----
        float count_material (int color) {

            /* Returns the accounted material value as a single float. */

            float cum;

            // iterate through corresponding occupation map
            for (auto const& x : board_main.get_occupation_map(color)) 
                cum += board_main.pieces.value_from_symbol(x.second);

            return cum;

        }

        // float evaluate_individual_positions (int color) {

        //     /* */

        // }



        /* test suites and simulations */
        void shannon_benchmark (int depth, bool visual=false) {
            
            /* An alias for shannon_number_simulation.
            Performs a time performance benchmark by stopping the time for each depth. */
            
            shannon_number_simulation (depth, visual);
            
        };

        void shannon_number_simulation (int depth, bool visual=false, bool clear=true) {
            
            /*
            Counts all possible move sequences i.e. the shannon number up to a specific depth
            which can be up to 10. If the provided depth exceeds this limit the simulation will 
            stop after depth 10.
            */

            // declare the expected position numbers for specific depth
            map<int, int> expected = {
                {1, 20},
                {2, 400},
                {3, 8902},
                {4, 197281},
                {5, 4865609},
                {6, 119060324},
                {7, 3195901860},
                {8, 84998978956},
                {9, 2439530234167},
                {10, 69352859712417}
            };

            console("Start sequence count simulation test:", "Engine");

            // setup test board from start
            board_test.load_starting_position(0);
            board_test.update_targets_and_moves(board_test.active_color);

            int s;
            string success;

            for (int d = 1; d <= depth; d++) {
                
                // run and benchmark the simulation up to depth d
                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                s = sequence_count_simulation(d, visual, clear);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                
                // determine output success
                if (s == expected[d])
                    success = "success";
                else 
                    success = "fail";
                    
                // output values
                cout << "Depth: " << d << "   Positions: " << s << "   Expected: " << expected[d] << 
                    "   Validation: " << success << "   Time: " << 
                    std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1e6 << "s " << endl;
            
            }
                
        }

    private:

        int sequence_count_simulation (int depth, bool visual=false, bool clear=true) {

            /* 
            An iterative sim approach to find the Shannon number for any specific depth. 
            This number of all possible positions does not account for transpositions of same pieces.
            The sim will start from current position.

            clear       Clear console after every board console.
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
            map<string, vector<string>> moves = board_test.get_moves(board_test.active_color);
            vector<string> targets;
            targets.reserve(28);
            string origin;
            
            snapshot move;

            // start
            // std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now(); 
            // console("0", "test");
            for (auto const& x : moves) {
                
                // extract keys and values i.e. origin coordinates 
                // and their corresponding target coordinates
                origin = x.first;
                targets = x.second;
                // console("1", "test");
                for (int i = 0; i < targets.size(); i++) {

                    // console("2", "test");
                    
                    // make an active move from origin to i-th target
                    move = board_test.move(origin, targets[i], 1);
                    
                    if (visual) {
                        if (clear)
                            clear_console();
                        board_test.show_board();
                    }
                    
                    // repeat iteratively
                    counts += sequence_count_simulation(depth-1, visual);
                    // console("4", "test");
                    board_test.revert(move);
                    // console("5", "test");
                    // show board in console
                    if (visual) {
                        if (clear)
                            clear_console();
                        board_test.show_board();
                    }
                        
                }
            }
            // console("6", "test");

            return counts;

        }
    
};