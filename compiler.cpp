#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include "lexer.cpp"
#include "parser.cpp"

using namespace std;

bool single_line_note;
bool multi_line_note;

void check_note_start(string group){
    size_t single_start_location = group.find("//");
    size_t multi_start_location = group.find("/*");
    if (single_start_location != string::npos){
        if (group != "//" && single_start_location != 0){
            process(group.substr(0, single_start_location));
        }
        single_line_note = true;
    }
    else if (multi_start_location != string::npos){
        if (group != "/*" && multi_start_location != 0){
            process(group.substr(0, multi_start_location));
        }
        multi_line_note = true;
    }
    else{
        process(group);
    }
}

void check_multi_note_end(string group){
    size_t multi_end_location = group.find("*/");
    if (multi_end_location != string::npos){
        if (group != "*/" && multi_end_location < group.length() - 2){
            process(group.substr(multi_end_location + 2));
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
        cout << line << endl;
        istringstream line_split(line);
        string group;
        while (line_split >> group){
            //cout << group << endl;
            if (!single_line_note && !multi_line_note){
                check_note_start(group);
                check_multi_note_end(group);
            }
            else if (!single_line_note && multi_line_note){
                check_multi_note_end(group);
            }
            else;
        }
        line_split.str("");
        single_line_note = false;
    }
    token_iter = tokens.begin();
    CompUnit();
    if (token_iter == tokens.end() && !multi_line_note){
        for (int i = 0; i < output.size(); i++){
            ir << output[i];
        }
    }
    else{
        exit(1);
    }
    input.close();
    ir.close();
}
