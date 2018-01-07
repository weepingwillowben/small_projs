#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <cassert>

using namespace std;

string read_file(string filename){
    std::ifstream t;
    int length;
    t.open(filename);      // open input file
    if(!t){
        assert(false && "file does not exist");
    }
    t.seekg(0, std::ios::end);    // go to the end
    length = t.tellg();           // report location (this is the length)
    t.seekg(0, std::ios::beg);    // go back to the beginning
    string buffer(length,' ');    // allocate memory for a buffer of appropriate dimension
    t.read(&buffer[0], length);       // read the whole file into the buffer
    t.close();                    // close file handle
    return buffer;
}
