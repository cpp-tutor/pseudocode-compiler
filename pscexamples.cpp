// pscexamples.cpp : group pseudocode programs into .js file for web interface
// Note: use ./pscexamples hello.psc chars.psc ... > ../pscexamples.js

#include <iostream>
#include <fstream>
#include <span>
#include <string>

using std::span;
using std::cout;
using std::ifstream;
using std::string;

int main(const int argc, const char **argv) {
    span<const char*> args{ argv + 1, argv + argc };
    cout << "pseudocode_examples = [ ";
    for (auto sep = ""; auto pscfile : args) {
        cout << sep << "{ name: \'" << pscfile << "\', code: `";
        ifstream file(pscfile);
        string code;
        getline(file, code, '\0');
        cout << code;
        cout << "` }";
        sep = ", ";
    }
    cout << " ];";
}