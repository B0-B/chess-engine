#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

void clear_console () {
    #if defined _WIN32
        system("cls");
        //clrscr(); // including header file : conio.h
    #elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
        system("clear");
        //std::cout<< u8"\033[2J\033[1;1H"; //Using ANSI Escape Sequences 
    #elif defined (__APPLE__)
        system("clear");
    #endif
};

bool contains_string (vector<string> v, string s) {
    
    /* Checks if a string-type vector contains a specific string. */
    
    for (int i = 0; i < v.size(); i++)
        if (s == v[i])
            return 1;
    return 0;

};

bool contains_substring (string s, string ss) {
    
    /* Check if a string contains a substring. */

    return s.find(ss) != std::string::npos;

};

void print (string message, string origin) {
    cout << "[" + origin + "]   " << message << endl;
};

string lower_case (string s) {
    
    /* Returns lowercase of string */

    for (int i = 0; i < s.size(); i++) {
        s[i] = tolower(s[i]);
    }
    return s;

}