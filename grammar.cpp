#include <iostream>
#include <cstdio>
#include <vector>

using namespace std;

vector<string>::iterator sym;

vector<string> items;

void CompUnit();
void FuncDef();
void FuncType();
void Ident();
void Block();
void Stmt();
void Exp();
void AddExp();
void MulExp();
void UnaryExp();
void PrimaryExp();
void UnaryOp();

void CompUnit(){
    FuncDef();
}

void FuncDef(){
    FuncType();
    Ident();
    if (*sym == "("){
        items.push_back("(");
        sym++;
        if (*sym == ")"){
            items.push_back(")");
            sym++;
        }
        else{
            exit(1);
        }
    }
    else{
        exit(1);
    }
    Block();
}

void FuncType(){
    if (*sym == "int"){
        items.push_back("define dso_local i32 ");
        sym++;
    }
    else{
        exit(1);
    }
}

void Ident(){
    if (*sym == "main"){
        items.push_back("@main");
        sym++;
    }
    else{
        exit(1);
    }
}

void Block(){
    if (*sym == "{"){
        items.push_back("{\n");
        sym++;
        Stmt();
        if (*sym == "}"){
            items.push_back("}\n");
            sym++;
        }
        else{
            exit(1);
        }
    }
    else{
        exit(1);
    }
}

int rtn, sign = 1;

void Stmt(){
    if (*sym == "return"){
        sym++;
        Exp();
        if (*sym == ";"){
            items.push_back("ret i32 " + to_string(sign * rtn) + "\n");
            sym++;
        }
        else{
            exit(1);
        }
    }
    else{
        exit(1);
    }
}

void Exp(){
    AddExp();
}

void AddExp(){
    MulExp();
}

void MulExp(){
    UnaryExp();
}

void UnaryExp(){
    if (*sym == "(" || (*sym).substr(0, 6) == "Number"){
        PrimaryExp();
    }
    else if (*sym == "+" || *sym == "-"){
        UnaryOp();
        UnaryExp();
    }
}

void PrimaryExp(){
    if (*sym == "("){
        sym++;
        Exp();
        if (*sym == ")"){
            sym++;
        }
        else{
            exit(1);
        }
    }
    else if ((*sym).substr(0, 6) == "Number"){
        rtn = stoi((*sym).substr(7));
        sym++;
    }
    else{
        exit(1);
    }
}

void UnaryOp(){
    if (*sym == "+" || *sym == "-"){
        if (*sym == "-"){
            sign = -sign;
        }
        sym++;
    }
    else{
        exit(1);
    }
}