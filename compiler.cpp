#include<iostream>
#include<fstream>
#include<sstream>
#include<cstdio>
#include<vector>

using namespace std;

vector<string> FuncDef;
vector<string> items;

void process_number(string word){
    bool decimal_octal=true;
    bool hexadecimal=(word.length()>2&&word.at(0)=='0'&&(word.at(1)=='x'||word.at(1)=='X'));
    for(int i=0;i<word.length();i++){
        if(!isdigit(word.at(i))){
            decimal_octal=false;
            break;
        }
    }
    if(hexadecimal){
        for(int i=2;i<word.length();i++){
            if(!isdigit(word.at(i))||!(word.at(i)>='a'&&word.at(i)<='f')||!(word.at(i)>='A'&&word.at(i)<='F')){
                hexadecimal=false;
                break;
            }
        }
    }
    if(decimal_octal||hexadecimal){
        items.push_back("i32 "+to_string(stoi(word)));
        FuncDef.push_back("Number");
    }
    else{
        exit(1);
    }
}

void process(string word){
    if(word=="int"){
        items.push_back("define dso_local i32 ");
        FuncDef.push_back("FuncType");
    }
    else if(word.length()>=4&&word.substr(0,4)=="main"){
        items.push_back("@main");
        FuncDef.push_back("Ident");
        if(word.length()>4){
            for(int i=4;i<word.length();i++){
                items.push_back(word.substr(i,1));
                FuncDef.push_back(word.substr(i,1));
            }
        }
    }
    else if(word=="("||word==")"||word=="{"||word=="}"||word==";"){
        items.push_back(word);
        FuncDef.push_back(word);
    }
    else if(word=="return"){
        items.push_back("    ret ");
        FuncDef.push_back(word);
    }
    else if(isdigit(word.at(0))){
        if(word.at(word.length()-1)==';'){
            process_number(word.substr(0,word.length()-1));
            items.push_back(";");
            FuncDef.push_back(";");
        }
        else{
            process_number(word);            
        }
    }
    else{
        exit(1);
    }
}

bool isCompUnit(){
    bool Stmt=(FuncDef[5]=="return")&&(FuncDef[6]=="Number")&&(FuncDef[7]==";");
    bool Block=(FuncDef[4]=="{")&&Stmt&&(FuncDef[8]=="}");
    return (FuncDef[0]=="FuncType")&&(FuncDef[1]=="Ident")&&(FuncDef[2]=="(")&&(FuncDef[3]==")")&&Block;
}

int main(int argc, char *argv[]){
    ifstream input;
    input.open(argv[1]);
    ofstream ir;
    ir.open(argv[2]);
    string line;
    while(getline(input,line)){
        istringstream line_split(line);
        string word;
        while (line_split >> word){
            process(word);
        }
        line_split.str("");
        items.push_back("\n");
    }
    if(isCompUnit()){
        for(int i=0;i<items.size();i++){
            ir<<items[i];
        }
    }
    else{
        exit(1);
    }
    input.close();
    ir.close();
}