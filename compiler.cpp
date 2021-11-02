#include<iostream>
#include<fstream>
#include<sstream>
#include<cstdio>
#include<vector>

using namespace std;

vector<string> FuncDef;
vector<string> items;

bool single_line_note;
bool multi_line_note;

void process_number(string word){
    bool decimal = (word.at(0) != '0');
    bool octal = (word.length() > 0 && word.at(0) == '0');
    bool hexadecimal = (word.length() > 2 && word.at(0) == '0' && (word.at(1) == 'x' || word.at(1) == 'X'));
    int base;
    if (decimal){
        for (int i = 0; i < word.length(); i++){
            if (!isdigit(word.at(i))){
                decimal = false;
                break;
            }
        }
        base = 10;
    }
    if (octal){
        for (int i = 1; i < word.length(); i++){
            if (word.at(i) < '0' || word.at(i) > '7'){
                octal = false;
                break;
            }
        }
        base = 8;
    }
    if (hexadecimal){
        for (int i = 2; i < word.length(); i++){
            if (!isdigit(word.at(i)) && !(word.at(i) >= 'a' && word.at(i) <= 'f') && !(word.at(i) >= 'A' && word.at(i) <= 'F')){
                hexadecimal = false;
                break;
            }
        }
        base = 16;
    }
    if (decimal || octal || hexadecimal){
        items.push_back("i32 " + to_string(stoi(word, 0, base)));
        FuncDef.push_back("Number");
    }
    else{
        exit(1);
    }
}

int letter(string word, int pos){
    string letters;
    while (pos < word.length() && (isalnum(word[pos]) || word[pos] == '_')){
        letters.push_back(word[pos]);
        pos++;
    }
    /*if (letters == "if"){
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
    /*else */if (letters == "return"){
        //printf("Return\n");
        items.push_back("ret ");
        FuncDef.push_back(letters);
    }
    else{
        //printf("Ident(%s)\n", letters.c_str());
        if (letters == "int"){
            items.push_back("define dso_local i32 ");
            FuncDef.push_back("FuncType");
        }
        else if (letters == "main"){
            items.push_back("@main");
            FuncDef.push_back("Ident");
        }
        else{
            exit(1);
        }
    }
    return letters.length();
}

int digit(string word, int pos){
    string number;
    while (pos < word.length() && isalnum(word[pos])){
        number.push_back(word[pos]);
        pos++;
    }
    //printf("Number(%s)\n", number.c_str());
    process_number(number);
    return number.length();
}

void process(string word){
    int pos = 0;
    while (pos < word.length()){
        if (isalpha(word[pos]) || word[pos] == '_'){
            pos += letter(word, pos);
        }
        else if (isdigit(word[pos])){
            pos += digit(word, pos);
        }
        else if (word[pos] == '(' || word[pos] == ')' || word[pos] == '{' || word[pos] == '}'){
            /*printf("LPar\n");
            printf("RPar\n");
            printf("LBrace\n");
            printf("RBrace\n");*/
            items.push_back(string(1, word[pos]));
            FuncDef.push_back(string(1, word[pos]));
            pos++;
        }
        /*else if (word[pos] == '+'){
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
        }*/
        else if (word[pos] == ';'){
            //printf("Semicolon\n");
            FuncDef.push_back(";");
            pos++;
        }
        else{
            //printf("Err\n");
            exit(1);
        }
    }
}

void check_note_start(string word){
    size_t single_start_location = word.find("//");
    size_t multi_start_location = word.find("/*");
    if (single_start_location != string::npos){
        if (word != "//" && single_start_location != 0){
            process(word.substr(0, single_start_location));
        }
        single_line_note = true;
    }
    else if (multi_start_location != string::npos){
        if (word != "/*" && multi_start_location != 0){
            process(word.substr(0, multi_start_location));
        }
        multi_line_note = true;
    }
    else{
        process(word);
    }
}

void check_multi_note_end(string word){
    size_t multi_end_location = word.find("*/");
    if (multi_end_location != string::npos){
        if (word != "*/" && multi_end_location < word.length() - 2){
            process(word.substr(multi_end_location + 2));
        }
        multi_line_note = false;
        return;
    }
}

bool isCompUnit(){
    bool Stmt = (FuncDef[5] == "return") && (FuncDef[6] == "Number") && (FuncDef[7] == ";");
    bool Block = (FuncDef[4] == "{") && Stmt && (FuncDef[8] == "}");
    return (FuncDef.size() == 9) && (FuncDef[0] == "FuncType") && (FuncDef[1] == "Ident") && (FuncDef[2] == "(") && (FuncDef[3] == ")") && Block;
}

int main(int argc, char *argv[]){
    ifstream input;
    input.open(argv[1]);
    ofstream ir;
    ir.open(argv[2]);
    string line;
    multi_line_note = false;
    while (getline(input, line)){
        //cout << line << endl;
        istringstream line_split(line);
        string word;
        while (line_split >> word){
            //cout << word << endl;
            if (!single_line_note && !multi_line_note){
                check_note_start(word);
                check_multi_note_end(word);
            }
            else if (!single_line_note && multi_line_note){
                check_multi_note_end(word);
            }
            else;
        }
        line_split.str("");
        items.push_back("\n");
        single_line_note = false;
    }
    if (isCompUnit() && !multi_line_note){
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
