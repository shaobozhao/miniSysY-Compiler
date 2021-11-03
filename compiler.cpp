#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include "lexer.cpp"
#include "grammar.cpp"

using namespace std;

//vector<string> syms;
//vector<string> items;

bool single_line_note;
bool multi_line_note;

void check_note_start(string token){
    size_t single_start_location = token.find("//");
    size_t multi_start_location = token.find("/*");
    if (single_start_location != string::npos){
        if (token != "//" && single_start_location != 0){
            process(token.substr(0, single_start_location));
        }
        single_line_note = true;
    }
    else if (multi_start_location != string::npos){
        if (token != "/*" && multi_start_location != 0){
            process(token.substr(0, multi_start_location));
        }
        multi_line_note = true;
    }
    else{
        process(token);
    }
}

void check_multi_note_end(string token){
    size_t multi_end_location = token.find("*/");
    if (multi_end_location != string::npos){
        if (token != "*/" && multi_end_location < token.length() - 2){
            process(token.substr(multi_end_location + 2));
        }
        multi_line_note = false;
        return;
    }
}

int main(int argc, char *argv[]){
    ifstream input;
    input.open(argv[1]);
    ofstream ir;
    ir.open(argv[2]);
    string line;
    while (getline(input, line)){
        //cout << line << endl;
        istringstream line_split(line);
        string token;
        while (line_split >> token){
            //cout << token << endl;
            if (!single_line_note && !multi_line_note){
                check_note_start(token);
                check_multi_note_end(token);
            }
            else if (!single_line_note && multi_line_note){
                check_multi_note_end(token);
            }
            else;
        }
        line_split.str("");
        single_line_note = false;
    }
    sym = syms.begin();
    CompUnit();
    if (sym == syms.end() && !multi_line_note){
        for (int i = 0; i < items.size(); i++){
            ir << items[i];
        }
    }
    else{
        exit(1);
    }
    input.close();
    ir.close();
}