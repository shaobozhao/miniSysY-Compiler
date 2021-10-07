#include<iostream>
#include<fstream>
#include<sstream>
#include<cstdio>

using namespace std;

int letter(string word, int pos){
    string letters;
    while (pos < word.length() && (isalnum(word[pos]) || word[pos] == '_')){
        letters.push_back(word[pos]);
        pos++;
    }
    if (letters == "if"){
        printf("If\n");
    }
    else if (letters == "else"){
        printf("Else\n");
    }
    else if (letters == "while"){
        printf("While\n");
    }
    else if (letters == "break"){
        printf("Break\n");
    }
    else if (letters == "continue"){
        printf("Continue\n");
    }
    else if (letters == "return"){
        printf("Return\n");
    }
    else{
        printf("Ident(%s)\n", letters.c_str());
    }
    return letters.length();
}

int digit(string word, int pos){
    string number;
    while (pos < word.length() && isdigit(word[pos])){
        number.push_back(word[pos]);
        pos++;
    }
    printf("Number(%s)\n", number.c_str());
    return number.length();
}

void token(string word){
    int pos = 0;
    while (pos < word.length()){
        if (isalpha(word[pos]) || word[pos] == '_'){
            pos += letter(word, pos);
        }
        else if (isdigit(word[pos])){
            pos += digit(word, pos);
        }
        else if (word[pos] == ';'){
            printf("Semicolon\n");
            pos++;
        }
        else if (word[pos] == '('){
            printf("LPar\n");
            pos++;
        }
        else if (word[pos] == ')'){
            printf("RPar\n");
            pos++;
        }
        else if (word[pos] == '{'){
            printf("LBrace\n");
            pos++;
        }
        else if (word[pos] == '}'){
            printf("RBrace\n");
            pos++;
        }
        else if (word[pos] == '+'){
            printf("Plus\n");
            pos++;
        }
        else if (word[pos] == '*'){
            printf("Mult\n");
            pos++;
        }
        else if (word[pos] == '/'){
            printf("Div\n");
            pos++;
        }
        else if (word[pos] == '<'){
            printf("Lt\n");
            pos++;
        }
        else if (word[pos] == '>'){
            printf("Gt\n");
            pos++;
        }
        else if (word[pos] == '='){
            if (pos < word.length() - 1 && word[pos + 1] == '='){
                printf("Eq\n");
                pos += 2;
            }
            else{
                printf("Assign\n");
                pos++;
            }
        }
        else{
            printf("Err\n");
            exit(0);
        }
    }
}

int main(int argc, char *argv[]){
    ifstream input;
    input.open(argv[1]);
    string line;
    while (getline(input, line)){
        istringstream line_split(line);
        string word;
        while (line_split >> word){
            token(word);
        }
        line_split.str("");
    }
    input.close();
}