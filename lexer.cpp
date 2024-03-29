#include <iostream>
#include <cstdio>
#include <vector>
#include "public.h"

using namespace std;

int process_word(string group, int pos){
    string word;
    while (pos < group.length() && (isalnum(group.at(pos)) || group.at(pos) == '_')){
        word.push_back(group.at(pos));
        pos++;
    }
    tokens.push_back(word);
    return word.length();
}

int process_number(string group, int pos){
    string number;
    while (pos < group.length() && isalnum(group.at(pos))){
        number.push_back(group.at(pos));
        pos++;
    }

    bool decimal = (number.at(0) != '0');
    bool octal = (number.length() > 0 && number.at(0) == '0');
    bool hexadecimal = (number.length() > 2 && number.at(0) == '0' && (number.at(1) == 'x' || number.at(1) == 'X'));
    int base;
    if (decimal){
        for (int i = 0; i < number.length(); i++){
            if (!isdigit(number.at(i))){
                decimal = false;
                break;
            }
        }
        base = 10;
    }
    if (octal){
        for (int i = 1; i < number.length(); i++){
            if (number.at(i) < '0' || number.at(i) > '7'){
                octal = false;
                break;
            }
        }
        base = 8;
    }
    if (hexadecimal){
        for (int i = 2; i < number.length(); i++){
            if (!isdigit(number.at(i)) && !(number.at(i) >= 'a' && number.at(i) <= 'f') && !(number.at(i) >= 'A' && number.at(i) <= 'F')){
                hexadecimal = false;
                break;
            }
        }
        base = 16;
    }
    if (decimal || octal || hexadecimal){
        tokens.push_back(to_string(stoi(number, 0, base)));
    }
    else{
        exit(1);
    }
    return number.length();
}

void process(string group){
    int pos = 0;
    while (pos < group.length()){
        if (isalpha(group.at(pos)) || group.at(pos) == '_'){
            pos += process_word(group, pos);
        }
        else if (isdigit(group.at(pos))){
            pos += process_number(group, pos);
        }
        else if (group.at(pos) == '(' || group.at(pos) == ')' || group.at(pos) == '[' || group.at(pos) == ']' || group.at(pos) == '{' || group.at(pos) == '}'){
            tokens.push_back(string(1, group.at(pos)));
            pos++;
        }
        else if (group.at(pos) == '+' || group.at(pos) == '-' || group.at(pos) == '*' || group.at(pos) == '/' || group.at(pos) == '%'){
            tokens.push_back(string(1, group.at(pos)));
            pos++;
        }
        else if (group.at(pos) == '<' || group.at(pos) == '>' || group.at(pos) == '=' || group.at(pos) == '!'){
            if (pos < group.length() - 1 && group.at(pos + 1) == '='){
                tokens.push_back(string(1, group.at(pos)) + "=");
                pos += 2;
            }
            else{
                tokens.push_back(string(1, group.at(pos)));
                pos++;
            }
        }
        else if (group.at(pos) == '&' || group.at(pos) == '|'){
            if (pos < group.length() - 1 && group.at(pos + 1) == group.at(pos)){
                tokens.push_back(string(2, group.at(pos)));
                pos += 2;
            }
            else{
                exit(1);
            }
        }
        else if (group.at(pos) == ',' || group.at(pos) == ';'){
            tokens.push_back(string(1, group.at(pos)));
            pos++;
        }
        else{
            exit(1);
        }
    }
}
