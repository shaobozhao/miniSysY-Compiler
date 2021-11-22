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

string rtn;
stack<string> registers;

void Stmt(){
    if (*sym == "return"){
        sym++;
        Exp();
        if (*sym == ";"){
            items.push_back("ret i32 " + rtn + "\n");
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

/*bool cmp(const string &left, const string &right){

}*/

void Exp(){
    /*stack<string> variety;
    while (*sym != ";" && sym != syms.end()){
        if ((*sym == "+") && (variety.empty() || !(isNumber(variety.top()) || variety.top() == ")"))){
            *sym = "plus";
        }
        if ((*sym == "-") && (variety.empty() || !(isNumber(variety.top()) || variety.top() == ")"))){
            *sym = "minus";
        }
        variety.push(*sym);
        cout << *sym + " " << endl;
        sym++;
    }*/
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
        if (op == "+"){
            items.push_back("%x" + to_string(registers.size() + 1) + " = add i32 " + var1 + ", " + var2 + "\n");
        }
        if (op == "-"){
            items.push_back("%x" + to_string(registers.size() + 1) + " = sub i32 " + var1 + ", " + var2 + "\n");
        }
        registers.push(var1 + op + var2);
        rtn = "%x" + to_string(registers.size());
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
        if (op == "*"){
            items.push_back("%x" + to_string(registers.size() + 1) + " = mul i32 " + var1 + ", " + var2 + "\n");
        }
        if (op == "/"){
            items.push_back("%x" + to_string(registers.size() + 1) + " = sdiv i32 " + var1 + ", " + var2 + "\n");
        }
        if (op == "%"){
            items.push_back("%x" + to_string(registers.size() + 1) + " = srem i32 " + var1 + ", " + var2 + "\n");
        }
        registers.push(var1 + op + var2);
        rtn = "%x" + to_string(registers.size());
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
            items.push_back("%x" + to_string(registers.size() + 1) + " = sub i32 0, " + rtn + "\n");
            registers.push("-" + rtn);
            rtn = "%x" + to_string(registers.size());
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
            //rtn = "%x" + to_string(registers.size());
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