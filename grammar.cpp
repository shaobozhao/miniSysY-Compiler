#include <iostream>
#include <cstdio>
#include <vector>
#include <stack>

using namespace std;

/* public */

vector<string>::iterator sym;

vector<string> items;

/*private */

struct element{
    string name;
    bool isConst;
    string type;
    string reg;
};

void CompUnit();
void Decl(vector<element> &elements);
void ConstDecl(vector<element> &elements);
void BType();
void ConstDef(vector<element> &elements);
void ConstInitVal(vector<element> &elements, element elem);
void ConstExp(vector<element> &elements, element elem);
void VarDecl(vector<element> &elements);
void VarDef(vector<element> &elements);
void InitVal(vector<element> &elements, element elem);
void FuncDef();
void FuncType();
void Block();
void BlockItem(vector<element> &elements);
void Stmt(vector<element> &elements);
void LVal(vector<element> &elements);
void Exp(vector<element> &elements, bool isConst);
void AddExp(vector<element> &elements, bool isConst);
void MulExp(vector<element> &elements, bool isConst);
void UnaryExp(vector<element> &elements, bool isConst);
void PrimaryExp(vector<element> &elements, bool isConst);
void UnaryOp();

void Ident();

element getElementByName(vector<element> &elements, string name){
    for (int i = 0; i < elements.size(); i++){
        if (elements[i].name == name){
            return elements[i];
            break;
        }
    }
    element false_rtn;
    false_rtn.type = "null";
    return false_rtn;
}

stack<string> registers;
string rtn;

bool isIdent(const string &str){
    bool nondigit = (isalpha(str.at(0)) || str.at(0) == '_');
    for (int i = 1; i < str.length(); i++){
        if (!isalnum(str.at(i)) && str.at(i) != '_'){
            return false;
        }
    }
    return nondigit && (str != "int") && (str != "return");
}

bool isNumber(const string &str){
    return str.find_first_not_of("0123456789") == string::npos;
}

void CompUnit(){
    FuncDef();
}

void Decl(vector<element> &elements){
    if(*sym == "const"){
        ConstDecl(elements);
    }
    else if(*sym == "int"){
        VarDecl(elements);
    }
    else{
        exit(1);
    }
}

void ConstDecl(vector<element> &elements){
    if(*sym == "const"){
        sym++;
        BType();
        ConstDef(elements);
        while(*sym == ","){
            sym++;
            ConstDef(elements);
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

void ConstDef(vector<element> &elements){
    string name = *sym;
    Ident();
    if(getElementByName(elements, name).type != "null"){
        exit(1);
    }
    element elem;
    elem.name = name;
    elem.isConst = true;
    elem.type = "int";
    elem.reg = "%" + to_string(registers.size());
    items.push_back("    " + elem.reg + " = alloca i32\n");
    //cout<<"    " + elem.reg + " = alloca i32"<<endl;
    registers.push("");
    if (*sym == "="){
        sym++;
    }
    else{
        exit(1);
    }
    ConstInitVal(elements, elem);
    elements.push_back(elem);
}

void ConstInitVal(vector<element> &elements, element elem){
    ConstExp(elements, elem);
}

void ConstExp(vector<element> &elements, element elem){
    AddExp(elements, true);
    items.push_back("    store i32 "+ rtn + ", i32* " + elem.reg + "\n");
    //cout<<"    store i32 "+ rtn + ", i32* " + elem.reg <<endl;
}

void VarDecl(vector<element> &elements){
    BType();
    VarDef(elements);
    while(*sym == ","){
        sym++;
        VarDef(elements);
    }
    if (*sym == ";"){
        sym++;
    }
    else{
        exit(1);
    }
}

void VarDef(vector<element> &elements){
    string name = *sym;
    Ident();
    if(getElementByName(elements, name).type != "null"){
        exit(1);
    }
    element elem;
    elem.name = name;
    elem.isConst = false;
    elem.type = "int";
    elem.reg = "%" + to_string(registers.size());
    items.push_back("    " + elem.reg + " = alloca i32\n");
    //cout<<"    " + elem.reg + " = alloca i32"<<endl;
    registers.push("");
    if(*sym == "="){
        sym++;
        InitVal(elements, elem);
    }
    elements.push_back(elem);
}

void InitVal(vector<element> &elements, element elem){
    Exp(elements, false);
    items.push_back("    store i32 "+ rtn + ", i32* " + elem.reg + "\n");
    //cout<<"    store i32 "+ rtn + ", i32* " + elem.reg<<endl;
}

void FuncDef(){
    FuncType();
    Ident();
    items.push_back("@main");
    //cout<<"@main";
    registers.push("main");
    if (*sym == "("){
        items.push_back("(");
        //cout<<"(";
        sym++;
        if (*sym == ")"){
            items.push_back(")");
            //cout<<")";
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
        //cout<<"define dso_local i32 ";
        sym++;
    }
    else{
        exit(1);
    }
}

void Block(){
    if (*sym == "{"){
        items.push_back("{\n");
        //cout<<"{"<<endl;
        sym++;
        vector<element> elements;
        while(*sym != "}"){
            BlockItem(elements);
        }
        items.push_back("}\n");
        //cout<<"}"<<endl;
        sym++;
    }
    else{
        exit(1);
    }
}

void BlockItem(vector<element> &elements){
    if(*sym == "const" || *sym == "int"){
        Decl(elements);
    }
    else{
        Stmt(elements);
    }
}

void Stmt(vector<element> &elements){
    if (isIdent(*sym)){
        LVal(elements);
        element elem = getElementByName(elements, *sym);
        if (elem.isConst){
            exit(1);
        }
        sym++;
        if (*sym == "="){
            sym++;
            Exp(elements, false);
            items.push_back("    store i32 "+ rtn + ", i32* " + elem.reg + "\n");
            //cout<<"    store i32 "+ rtn + ", i32* " + elem.reg<<endl;
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
        Exp(elements, false);
        if (*sym == ";"){
            items.push_back("    ret i32 " + rtn + "\n");
            //cout<<"    ret i32 " + rtn<<endl;
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
        Exp(elements, false);
        if (*sym == ";"){
            sym++;
        }
        else {
            exit(1);
        }
    }
}

void LVal(vector<element> &elements){
    if(getElementByName(elements, *sym).type == "null"){
        exit(1);
    }
}

void Exp(vector<element> &elements, bool isConst){
    AddExp(elements, isConst);
}

void AddExp(vector<element> &elements, bool isConst){
    MulExp(elements, isConst);
    string var1, var2;
    while (*sym == "+" || *sym == "-"){
        var1 = rtn;
        string op = *sym;
        sym++;
        MulExp(elements, isConst);
        var2 = rtn;
        int new_reg = registers.size();
        if (op == "+"){
            items.push_back("    %" + to_string(new_reg) + " = add i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %" + to_string(new_reg) + " = add i32 " + var1 + ", " + var2<<endl;
        }
        if (op == "-"){
            items.push_back("    %" + to_string(new_reg) + " = sub i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %" + to_string(new_reg) + " = sub i32 " + var1 + ", " + var2<<endl;
        }
        registers.push(var1 + op + var2);
        rtn = "%" + to_string(new_reg);
    }
}

void MulExp(vector<element> &elements, bool isConst){
    UnaryExp(elements, isConst);
    string var1, var2;
    while (*sym == "*" || *sym == "/" || *sym == "%"){
        var1 = rtn;
        string op = *sym;
        sym++;
        UnaryExp(elements, isConst);
        var2 = rtn;
        int new_reg = registers.size();
        if (op == "*"){
            items.push_back("    %" + to_string(new_reg) + " = mul i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %" + to_string(new_reg) + " = mul i32 " + var1 + ", " + var2<<endl;
        }
        if (op == "/"){
            items.push_back("    %" + to_string(new_reg) + " = sdiv i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %" + to_string(new_reg) + " = sdiv i32 " + var1 + ", " + var2<<endl;
        }
        if (op == "%"){
            items.push_back("    %" + to_string(new_reg) + " = srem i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %" + to_string(new_reg) + " = srem i32 " + var1 + ", " + var2<<endl;
        }
        registers.push(var1 + op + var2);
        rtn = "%" + to_string(new_reg);
    }
}

void UnaryExp(vector<element> &elements, bool isConst){
    if (*sym == "(" || isNumber(*sym) || isIdent(*sym)){
        PrimaryExp(elements, isConst);
    }
    else if (*sym == "+" || *sym == "-"){
        bool minus = *sym == "-";
        UnaryOp();
        UnaryExp(elements, isConst);
        if (minus){
            int new_reg = registers.size();
            items.push_back("    %" + to_string(new_reg) + " = sub i32 0, " + rtn + "\n");
            //cout<<"    %" + to_string(new_reg) + " = sub i32 0, " + rtn<<endl;
            registers.push("-" + rtn);
            rtn = "%" + to_string(new_reg);
        }
    }
    else{
        exit(1);
    }
}

void PrimaryExp(vector<element> &elements, bool isConst){
    if (*sym == "("){
        sym++;
        Exp(elements, isConst);
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
    else if (isIdent(*sym)){
        LVal(elements);
        element elem = getElementByName(elements, *sym);
        if (isConst){
            if (!elem.isConst){
                exit(1);
            }
        }
        int new_reg = registers.size();
        items.push_back("    %" + to_string(new_reg) + " = load i32, i32* " + elem.reg + "\n");
        //cout<<"    %" + to_string(new_reg) + " = load i32, i32* " + elem.reg<<endl;
        registers.push(elem.reg);
        rtn =  "%" + to_string(new_reg);
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
    if (isIdent(*sym)){
        sym++;
    }
    else{
        exit(1);
    }
}