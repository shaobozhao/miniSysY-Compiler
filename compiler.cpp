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

int process_word(string token, int pos){
    string word;
    while (pos < token.length() && (isalnum(token.at(pos)) || token.at(pos) == '_')){
        word.push_back(token.at(pos));
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
    /*else */if (word == "return"){
        items.push_back("ret ");
        FuncDef.push_back(word);
    }
    else{
        if (word == "int"){
            items.push_back("define dso_local i32 ");
            FuncDef.push_back("FuncType");
        }
        else if (word == "main"){
            items.push_back("@main");
            FuncDef.push_back("Ident");
        }
        else{
            exit(1);
        }
    }
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
        items.push_back("i32 " + to_string(stoi(number, 0, base)));
        FuncDef.push_back("Number");
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
        else if (token.at(pos) == '(' || token.at(pos) == ')' || token.at(pos) == '{' || token.at(pos) == '}'){
            items.push_back(string(1, token.at(pos)));
            FuncDef.push_back(string(1, token.at(pos)));
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
        else if (token.at(pos) == ';'){
            FuncDef.push_back(";");
            pos++;
        }
        else{
            exit(1);
        }
    }
}

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
