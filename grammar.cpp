#include <iostream>
#include <cstdio>
#include <vector>
#include <stack>
#include <algorithm>

using namespace std;

/* public */

vector<string>::iterator sym;

vector<string> output;

/*private */

struct element{
    string name;
    bool isConst;
    string type;
    string reg;
};

struct function{
    string name;
    string type;
    int argc;
    vector<string> argv;
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
void FuncRParams(vector<element> &elements, function func, string &params, bool isConst);
void UnaryOp();

void Ident();

bool is_element(vector<element> &elements, const string& str){
    auto elem_iter = find_if(elements.begin(), elements.end(), [str](element elem){
        if (elem.name == str){
            return true;
        }
        else{
            return false;
        }
    });
    if (elem_iter != elements.end()){
        return true;
    }
    else{
        return false;
    }
}

element get_elem_by_name(vector<element> &elements, string name){
    element elem;
    for (int i = 0; i < elements.size(); i++){
        if (elements[i].name == name){
            elem = elements[i];
            break;
        }
    }
    return elem;
}

const function getint = {.name = "getint", .type = "int", .argc = 0, .argv = {}}, 
           getch = {.name = "getch", .type = "int", .argc = 0, .argv = {}}, 
           getarray = {.name = "getarray", .type = "int", .argc = 1, .argv = {"int*"}}, 
           putint = {.name = "putint", .type = "void", .argc = 1, .argv = {"int"}}, 
           putch = {.name = "putch", .type = "void", .argc = 1, .argv = {"int"}}, 
           putarray = {.name = "putarray", .type = "void", .argc = 2, .argv = {"int", "int*"}};

const vector<function> miniSysY = {getint, getch, getarray, putint, putch, putarray};

vector<function> functions;

stack<string> memory;
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

bool isFunc(const string &str){
    auto func_iter = find_if(functions.begin(), functions.end(), [str](function func){
        if (func.name == str){
            return true;
        }
        else{
            return false;
        }
    });
    auto call_iter = find_if(miniSysY.begin(), miniSysY.end(), [str](function func){
        if (func.name == str){
            return true;
        }
        else{
            return false;
        }
    });
    if (func_iter != functions.end()){
        return true;
    }
    else if (call_iter != miniSysY.end()){
        string call_line;
        if ((*call_iter).type == "int"){
            call_line = "declare i32 @" + (*call_iter).name + "()\n";
        }
        else{
            call_line = "declare void @" + (*call_iter).name + "(i32)\n";
        }
        output.insert(output.begin(), call_line);
        functions.push_back(*call_iter);
        return true;
    }
    else{
        return false;
    }
}

function get_func_by_name(string name){
    function func;
    for (int i = 0; i < functions.size(); i++){
        if (functions[i].name == name){
            func = functions[i];
            break;
        }
    }
    return func;
}

bool isnum(const string &str){
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
    if(is_element(elements, name)){
        exit(1);
    }
    element elem;
    elem.name = name;
    elem.isConst = true;
    elem.type = "int";
    elem.reg = "%" + to_string(memory.size());
    output.push_back("    " + elem.reg + " = alloca i32\n");
    //cout<<"    " + elem.reg + " = alloca i32"<<endl;
    memory.push("");
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
    output.push_back("    store i32 "+ rtn + ", i32* " + elem.reg + "\n");
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
    if(is_element(elements, name)){
        exit(1);
    }
    element elem;
    elem.name = name;
    elem.isConst = false;
    elem.type = "int";
    elem.reg = "%" + to_string(memory.size());
    output.push_back("    " + elem.reg + " = alloca i32\n");
    //cout<<"    " + elem.reg + " = alloca i32"<<endl;
    memory.push("");
    if(*sym == "="){
        sym++;
        InitVal(elements, elem);
    }
    elements.push_back(elem);
}

void InitVal(vector<element> &elements, element elem){
    Exp(elements, false);
    output.push_back("    store i32 "+ rtn + ", i32* " + elem.reg + "\n");
    //cout<<"    store i32 "+ rtn + ", i32* " + elem.reg<<endl;
}

void FuncDef(){
    FuncType();
    Ident();
    output.push_back("@main");
    //cout<<"@main";
    memory.push("main");
    if (*sym == "("){
        output.push_back("(");
        //cout<<"(";
        sym++;
        if (*sym == ")"){
            output.push_back(")");
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
        output.push_back("define dso_local i32 ");
        //cout<<"define dso_local i32 ";
        sym++;
    }
    else{
        exit(1);
    }
}

void Block(){
    if (*sym == "{"){
        output.push_back("{\n");
        //cout<<"{"<<endl;
        sym++;
        vector<element> elements;
        while(*sym != "}"){
            BlockItem(elements);
            /*if(sym == syms.end()){
                exit(1);
            }*/
        }
        output.push_back("}\n");
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
    if (isIdent(*sym) && !isFunc(*sym)){
        LVal(elements);
        element elem = get_elem_by_name(elements, *sym);
        if (elem.isConst){
            exit(1);
        }
        sym++;
        if (*sym == "="){
            sym++;
            Exp(elements, false);
            output.push_back("    store i32 "+ rtn + ", i32* " + elem.reg + "\n");
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
            output.push_back("    ret i32 " + rtn + "\n");
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
    if(!is_element(elements, *sym)){
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
        int new_reg = memory.size();
        if (op == "+"){
            output.push_back("    %" + to_string(new_reg) + " = add i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %" + to_string(new_reg) + " = add i32 " + var1 + ", " + var2<<endl;
        }
        if (op == "-"){
            output.push_back("    %" + to_string(new_reg) + " = sub i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %" + to_string(new_reg) + " = sub i32 " + var1 + ", " + var2<<endl;
        }
        memory.push(var1 + op + var2);
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
        int new_reg = memory.size();
        if (op == "*"){
            output.push_back("    %" + to_string(new_reg) + " = mul i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %" + to_string(new_reg) + " = mul i32 " + var1 + ", " + var2<<endl;
        }
        if (op == "/"){
            output.push_back("    %" + to_string(new_reg) + " = sdiv i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %" + to_string(new_reg) + " = sdiv i32 " + var1 + ", " + var2<<endl;
        }
        if (op == "%"){
            output.push_back("    %" + to_string(new_reg) + " = srem i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %" + to_string(new_reg) + " = srem i32 " + var1 + ", " + var2<<endl;
        }
        memory.push(var1 + op + var2);
        rtn = "%" + to_string(new_reg);
    }
}

void UnaryExp(vector<element> &elements, bool isConst){
    if (*sym == "(" || isnum(*sym) || isIdent(*sym)){
        if (isFunc(*sym)){
            function func = get_func_by_name(*sym);
            sym++;
            if (*sym == "("){
                sym++;
                string params;
                FuncRParams(elements, func, params, isConst);
                if (*sym == ")"){
                    if (func.type == "int"){
                        int new_reg = memory.size();
                        output.push_back("    %" + to_string(new_reg) + " = call i32 @" + func.name + "(" + params + ")\n");
                        //cout<<"    %" + to_string(new_reg) + " = call i32 @" + func.name + "(" + params + ")"<<endl;
                        memory.push(func.name + "(" + params + ")");
                        rtn = "%" + to_string(new_reg);
                    }
                    else{
                        output.push_back("    call void @" + func.name + "(" + params + ")\n");
                        //cout<<"    call void @" + func.name + "(" + params + ")"<<endl;
                    }
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
        else{
            PrimaryExp(elements, isConst);
        }
    }
    else if (*sym == "+" || *sym == "-"){
        bool minus = *sym == "-";
        UnaryOp();
        UnaryExp(elements, isConst);
        if (minus){
            int new_reg = memory.size();
            output.push_back("    %" + to_string(new_reg) + " = sub i32 0, " + rtn + "\n");
            //cout<<"    %" + to_string(new_reg) + " = sub i32 0, " + rtn<<endl;
            memory.push("-" + rtn);
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
    else if (isnum(*sym)){
        rtn = *sym;
        sym++;
    }
    else if (isIdent(*sym)){
        LVal(elements);
        element elem = get_elem_by_name(elements, *sym);
        if (isConst){
            if (!elem.isConst){
                exit(1);
            }
        }
        int new_reg = memory.size();
        output.push_back("    %" + to_string(new_reg) + " = load i32, i32* " + elem.reg + "\n");
        //cout<<"    %" + to_string(new_reg) + " = load i32, i32* " + elem.reg<<endl;
        memory.push(elem.reg);
        rtn =  "%" + to_string(new_reg);
        sym++;
    }
    else{
        exit(1);
    }
}

/* getarray & putarray unfinished */
void FuncRParams(vector<element> &elements, function func, string &params, bool isConst){
    int argc = 0;
    while(*sym != ")"){
        Exp(elements, isConst);
        params += ("i32 " + rtn);
        argc++;
        if (*sym == ","){
            params += ", ";
            sym++;
        }
    }
    if (argc != func.argc){
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
