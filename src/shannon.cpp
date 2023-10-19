/* Shannon number simulation test */

#include "engine.h"
#include <stdio.h>
#include <iostream>

using namespace std;

int main (void) {

    Engine engine;
    
    // shannon number test (number of positions for diven depth)
    int depth = 4;
    engine.shannon_number_simulation(depth, 0, 0);

    return 0;

}