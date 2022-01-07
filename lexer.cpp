#include <iostream>
#include <cstdio>
#include <vector>
#include "public.h"

using namespace std;

int process_word(string token, int pos){
    string word;
    while (pos < token.length() && (isalnum(token.at(pos)) || token.at(pos) == '_')){
        word.push_back(token.at(pos));
        pos++;
    }
    syms.push_back(word);
    return word.length();
}

int process_number(string token, int pos){
    string number;
    while (pos < token.length() && isalnum(token.at(pos))){
        number.push_back(token.at(pos));
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
        syms.push_back(to_string(stoi(number, 0, base)));
    }
    else{
        exit(1);
    }
    return number.length();
}

void process(string token){
    int pos = 0;
    while (pos < token.length()){
        if (isalpha(token.at(pos)) || token.at(pos) == '_'){
            pos += process_word(token, pos);
        }
        else if (isdigit(token.at(pos))){
            pos += process_number(token, pos);
        }
        else if (token.at(pos) == '(' || token.at(pos) == ')' || token.at(pos) == '[' || token.at(pos) == ']' || token.at(pos) == '{' || token.at(pos) == '}'){
            syms.push_back(string(1, token.at(pos)));
            pos++;
        }
        else if (token.at(pos) == '+' || token.at(pos) == '-' || token.at(pos) == '*' || token.at(pos) == '/' || token.at(pos) == '%'){
            syms.push_back(string(1, token.at(pos)));
            pos++;
        }
        else if (token.at(pos) == '<' || token.at(pos) == '>' || token.at(pos) == '=' || token.at(pos) == '!'){
            if (pos < token.length() - 1 && token.at(pos + 1) == '='){
                syms.push_back(string(1, token.at(pos)) + "=");
                pos += 2;
            }
            else{
                syms.push_back(string(1, token.at(pos)));
                pos++;
            }
        }
        else if (token.at(pos) == '&' || token.at(pos) == '|'){
            if (pos < token.length() - 1 && token.at(pos + 1) == token.at(pos)){
                syms.push_back(string(2, token.at(pos)));
                pos += 2;
            }
            else{
                exit(1);
            }
        }
        else if (token.at(pos) == ',' || token.at(pos) == ';'){
            syms.push_back(string(1, token.at(pos)));
            pos++;
        }
        else{
            exit(1);
        }
    }
}
