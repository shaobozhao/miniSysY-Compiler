#include <iostream>
#include <cstdio>
#include <vector>
#include <stack>

using namespace std;

vector<string>::iterator sym;

vector<string> items;

void CompUnit();
void Decl();
void ConstDecl();
void BType();
void ConstDef();
void ConstInitVal();
void ConstExp();
void VarDecl();
void VarDef();
void InitVal();
void FuncDef();
void FuncType();
void Block();
void BlockItem();
void Stmt();
void LVal();
void Exp();
void AddExp();
void MulExp();
void UnaryExp();
void PrimaryExp();
void UnaryOp();

void Ident();

stack<string> registers;
string rtn;

bool isIdent(const string &str){
    return (isalpha(str.at(0)) || str.at(0) == '_') && (str.find_first_not_of("0123456789") == string::npos);
}

bool isNumber(const string &str){
    return str.find_first_not_of("0123456789") == string::npos;
}

void CompUnit(){
    FuncDef();
}

void Decl(){
    if(*sym == "const"){
        ConstDecl();
    }
    else if(*sym == "int"){
        VarDecl();
    }
    else{
        exit(1);
    }
}

void ConstDecl(){
    if(*sym == "const"){
        sym++;
        BType();
        ConstDef();
        while(*sym == ","){
            sym++;
            ConstDef();
        }
        if(*sym == ";"){
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

void BType(){
    if(*sym == "int"){
        sym++;
    }
    else{
        exit(1);
    }
}

void ConstDef(){
    Ident();
    if (*sym == "="){
        sym++;
    }
    ConstInitVal();
}

void ConstInitVal(){
    ConstExp();
}

void ConstExp(){
    AddExp();
}

void VarDecl(){
    BType();
    VarDef();
    while(*sym == ","){
        sym++;
        VarDef();
    }
    if (*sym == ";"){
        sym++;
    }
    else{
        exit(1);
    }
}

void VarDef(){
    Ident();
    if(*sym == "="){
        sym++;
        InitVal();
    }
}

void FuncDef(){
    FuncType();
    Ident();
    items.push_back("@main");
    registers.push("main");
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

void Block(){
    if (*sym == "{"){
        items.push_back("{\n");
        sym++;
        while(*sym != "}"){
            BlockItem();
        }
        items.push_back("}\n");
        sym++;
    }
    else{
        exit(1);
    }
}

void BlockItem(){
    if(*sym == "const" || *sym == "int"){
        Decl();
    }
    else{
        Stmt();
    }
}

void Stmt(){
    if (isIdent(*sym)){
        LVal();
        if (*sym == "="){
            sym++;
            Exp();
            if (*sym == ";"){
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
    else if (*sym == "return"){
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
    else if (*sym == ";"){
        sym++;
    }
    else{
        Exp();
        if (*sym == ";"){
            sym++;
        }
        else {
            exit(1);
        }
    }
}

void LVal(){
    Ident();
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

void Ident(){
    if (*sym == "main"){
        
        sym++;
    }
    else{
        exit(1);
    }
}