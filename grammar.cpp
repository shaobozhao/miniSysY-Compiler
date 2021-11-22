#include <iostream>
#include <cstdio>
#include <vector>
#include <stack>

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

stack<string> registers;

void Block(){
    if (*sym == "{"){
        items.push_back("{\n");
        registers.push("main");
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

string rtn;

void Stmt(){
    if (*sym == "return"){
        sym++;
        Exp();
        if (*sym == ";"){
            items.push_back("    ret i32 " + rtn + "\n");
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

bool isNumber(const string &str){
    return str.find_first_not_of("0123456789") == string::npos;
}

void Exp(){
    AddExp();
}

void AddExp(){
    MulExp();
    string var1, var2;
    while (*sym == "+" || *sym == "-"){
        var1 = rtn;
        string op = *sym;
        sym++;
        MulExp();
        var2 = rtn;
        int new_reg = registers.size();
        if (op == "+"){
            items.push_back("    %" + to_string(new_reg) + " = add i32 " + var1 + ", " + var2 + "\n");
        }
        if (op == "-"){
            items.push_back("    %" + to_string(new_reg) + " = sub i32 " + var1 + ", " + var2 + "\n");
        }
        registers.push(var1 + op + var2);
        rtn = "%" + to_string(new_reg);
    }
}

void MulExp(){
    UnaryExp();
    string var1, var2;
    while (*sym == "*" || *sym == "/" || *sym == "%"){
        var1 = rtn;
        string op = *sym;
        sym++;
        UnaryExp();
        var2 = rtn;
        int new_reg = registers.size();
        if (op == "*"){
            items.push_back("    %" + to_string(new_reg) + " = mul i32 " + var1 + ", " + var2 + "\n");
        }
        if (op == "/"){
            items.push_back("    %" + to_string(new_reg) + " = sdiv i32 " + var1 + ", " + var2 + "\n");
        }
        if (op == "%"){
            items.push_back("    %" + to_string(new_reg) + " = srem i32 " + var1 + ", " + var2 + "\n");
        }
        registers.push(var1 + op + var2);
        rtn = "%" + to_string(new_reg);
    }
}

void UnaryExp(){
    if (*sym == "(" || isNumber(*sym)){
        PrimaryExp();
    }
    else if (*sym == "+" || *sym == "-"){
        bool minus = *sym == "-";
        UnaryOp();
        UnaryExp();
        if (minus){
            int new_reg = registers.size();
            items.push_back("    %" + to_string(new_reg) + " = sub i32 0, " + rtn + "\n");
            registers.push("-" + rtn);
            rtn = "%" + to_string(new_reg);
        }
    }
    else{
        exit(1);
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
    else if (isNumber(*sym)){
        rtn = *sym;
        sym++;
    }
    else{
        exit(1);
    }
}

void UnaryOp(){
    if (*sym == "+" || *sym == "-"){
        sym++;
    }
    else{
        exit(1);
    }
}
