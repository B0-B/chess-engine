/* Shannon number simulation test */

#include "engine.h"
#include <stdio.h>
#include <iostream>

using namespace std;

int main (void) {

    Engine engine;
    // shannon number test (number of positions for diven depth)
    int depth = 3;
    engine.shannon_number_simulation(depth, 0, 0);
    engine.board_test.show_call_stats();
    return 0;
}