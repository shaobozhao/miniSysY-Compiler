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
    int level;
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
void FuncDef(vector<element> &elements);
void FuncType();
void Block(vector<element> &elements);
void BlockItem(vector<element> &elements);
void Stmt(vector<element> &elements);
void Exp(vector<element> &elements, bool isConst);
void Cond(vector<element> &elements, int block_exec, int block_false_exec);
void LVal(vector<element> &elements);
void PrimaryExp(vector<element> &elements, bool isConst);
void UnaryExp(vector<element> &elements, bool isConst);
void UnaryOp();
void FuncRParams(vector<element> &elements, function func, string &params, bool isConst);
void MulExp(vector<element> &elements, bool isConst);
void AddExp(vector<element> &elements, bool isConst);
void RelExp(vector<element> &elements);
void EqExp(vector<element> &elements);
void LAndExp(vector<element> &elements, int upper_block_next_exec);
void LOrExp(vector<element> &elements, int block_exec, int block_false_exec);
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

int level;
stack<string> memory;
string rtn;

bool isIdent(const string &str){
    bool nondigit = (isalpha(str.at(0)) || str.at(0) == '_');
    for (int i = 1; i < str.length(); i++){
        if (!isalnum(str.at(i)) && str.at(i) != '_'){
            return false;
        }
    }
    bool reserved = str != "const" && str != "int" && str != "if" && str != "else" && str != "return";
    return nondigit && reserved;
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
    vector<element> elements;
    FuncDef(elements);
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
    elem.level = level;
    elem.reg = "%x" + to_string(memory.size());
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
    elem.level = level;
    elem.reg = "%x" + to_string(memory.size());
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

void FuncDef(vector<element> &elements){
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
    if (*sym == "{"){
        output.push_back("{\n");
        //cout<<"{"<<endl;
        Block(elements);
        output.push_back("}\n");
        //cout<<"}"<<endl;
    }
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

void Block(vector<element> &elements){
    if (*sym == "{"){
        level++;
        vector<element> sub_elements;
        sub_elements.assign(elements.begin(), elements.end());
        sym++;
        while(*sym != "}"){
            BlockItem(sub_elements);
            /*if(sym == syms.end()){
                exit(1);
            }*/
        }
        level--;
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
    else if (*sym == "{"){
        Block(elements);
    }
    else if (*sym == "if"){
        sym++;
        if (*sym == "("){
            sym++;
            int block_exec = memory.size();
            memory.push("block_exec");
            int block_false_exec = memory.size();
            memory.push("block_false_exec");
            int block_out = memory.size();
            memory.push("block_out");
            Cond(elements, block_exec, block_false_exec);
            if (*sym == ")"){
                sym++;
                output.push_back("\n" + to_string(block_exec) + ":\n");
                //cout<<"\n" + to_string(block_exec) + ":"<<endl;
                Stmt(elements);
                output.push_back("    br label %x" + to_string(block_false_exec) + "\n");
                //cout<<"    br label %x" + to_string(block_false_exec)<<endl;
                output.push_back("\n" + to_string(block_false_exec) + ":\n");
                //cout<<"\n" + to_string(block_false_exec) + ":"<<endl;
                if (*sym == "else"){
                    sym++;
                    Stmt(elements);
                }
                output.push_back("    br label %x" + to_string(block_out) + "\n");
                //cout<<"    br label %x" + to_string(block_out)<<endl;
                output.push_back("\n" + to_string(block_out) + ":\n");
                //cout<<"\n" + to_string(block_out) + ":"<<endl;
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

void Exp(vector<element> &elements, bool isConst){
    AddExp(elements, isConst);
}

void Cond(vector<element> &elements, int block_exec, int block_false_exec){
    LOrExp(elements, block_exec, block_false_exec);
}

void LVal(vector<element> &elements){
    if(!is_element(elements, *sym)){
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
        output.push_back("    %x" + to_string(new_reg) + " = load i32, i32* " + elem.reg + "\n");
        //cout<<"    %x" + to_string(new_reg) + " = load i32, i32* " + elem.reg<<endl;
        memory.push(elem.reg);
        rtn =  "%x" + to_string(new_reg);
        sym++;
    }
    else{
        exit(1);
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
                        output.push_back("    %x" + to_string(new_reg) + " = call i32 @" + func.name + "(" + params + ")\n");
                        //cout<<"    %x" + to_string(new_reg) + " = call i32 @" + func.name + "(" + params + ")"<<endl;
                        memory.push(func.name + "(" + params + ")");
                        rtn = "%x" + to_string(new_reg);
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
    else if (*sym == "+" || *sym == "-" || *sym == "!"){
        bool minus = *sym == "-";
        bool inverse = *sym == "!";
        UnaryOp();
        UnaryExp(elements, isConst);
        if (minus){
            int new_reg = memory.size();
            output.push_back("    %x" + to_string(new_reg) + " = sub i32 0, " + rtn + "\n");
            //cout<<"    %x" + to_string(new_reg) + " = sub i32 0, " + rtn<<endl;
            memory.push("-" + rtn);
            rtn = "%x" + to_string(new_reg);
        }
        if (inverse){
            int new_reg = memory.size();
            output.push_back("    %x" + to_string(new_reg) + " = icmp eq i32 " + rtn + ", 0\n");
            //cout<<"    %x" + to_string(new_reg) + " = icmp eq i32 " + rtn + ", 0"<<endl;
            memory.push("!" + rtn);
            rtn = "%x" + to_string(new_reg);
            new_reg = memory.size();
            output.push_back("    %x" + to_string(new_reg) + " = zext i1 " + rtn + " to i32\n");
            //cout<<"    %x" + to_string(new_reg) + " = zext i1 " + rtn + " to i32"<<endl;
            memory.push(rtn + "to i32");
            rtn = "%x" + to_string(new_reg);
        }
    }
    else{
        exit(1);
    }
}

void UnaryOp(){
    if (*sym == "+" || *sym == "-" || *sym == "!"){
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

void MulExp(vector<element> &elements, bool isConst){
    UnaryExp(elements, isConst);
    string var1, var2;
    while (*sym == "*" || *sym == "/" || *sym == "%x"){
        var1 = rtn;
        string op = *sym;
        sym++;
        UnaryExp(elements, isConst);
        var2 = rtn;
        int new_reg = memory.size();
        if (op == "*"){
            output.push_back("    %x" + to_string(new_reg) + " = mul i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %x" + to_string(new_reg) + " = mul i32 " + var1 + ", " + var2<<endl;
        }
        if (op == "/"){
            output.push_back("    %x" + to_string(new_reg) + " = sdiv i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %x" + to_string(new_reg) + " = sdiv i32 " + var1 + ", " + var2<<endl;
        }
        if (op == "%x"){
            output.push_back("    %x" + to_string(new_reg) + " = srem i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %x" + to_string(new_reg) + " = srem i32 " + var1 + ", " + var2<<endl;
        }
        memory.push(var1 + op + var2);
        rtn = "%x" + to_string(new_reg);
    }
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
            output.push_back("    %x" + to_string(new_reg) + " = add i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %x" + to_string(new_reg) + " = add i32 " + var1 + ", " + var2<<endl;
        }
        if (op == "-"){
            output.push_back("    %x" + to_string(new_reg) + " = sub i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %x" + to_string(new_reg) + " = sub i32 " + var1 + ", " + var2<<endl;
        }
        memory.push(var1 + op + var2);
        rtn = "%x" + to_string(new_reg);
    }
}

void RelExp(vector<element> &elements){
    AddExp(elements, false);
    string var1, var2, var_temp = rtn;
    while (*sym == "<" || *sym == ">" || *sym == "<=" || *sym == ">="){
        var1 = var_temp;
        string op = *sym;
        sym++;
        AddExp(elements, false);
        var2 = rtn;
        var_temp = var2;
        int new_reg = memory.size();
        if (op == "<"){
            output.push_back("    %x" + to_string(new_reg) + " = icmp slt i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %x" + to_string(new_reg) + " = icmp slt i32 " + var1 + ", " + var2<<endl;
        }
        if (op == ">"){
            output.push_back("    %x" + to_string(new_reg) + " = icmp sgt i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %x" + to_string(new_reg) + " = icmp sgt i32 " + var1 + ", " + var2<<endl;
        }
        if (op == "<="){
            output.push_back("    %x" + to_string(new_reg) + " = icmp sle i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %x" + to_string(new_reg) + " = icmp sle i32 " + var1 + ", " + var2<<endl;
        }
        if (op == ">="){
            output.push_back("    %x" + to_string(new_reg) + " = icmp sge i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %x" + to_string(new_reg) + " = icmp sge i32 " + var1 + ", " + var2<<endl;
        }
        memory.push(var1 + op + var2);
        /*rtn = "%x" + to_string(new_reg);
        new_reg = memory.size();
        output.push_back("    %x" + to_string(new_reg) + " = zext i1 " + rtn + " to i32\n");
        //cout<<"    %x" + to_string(new_reg) + " = zext i1 " + rtn + " to i32"<<endl;
        memory.push(rtn + "to i32");
        rtn = "%x" + to_string(new_reg);*/
    }
    
}

void EqExp(vector<element> &elements){
    RelExp(elements);
    string var1, var2, var_temp = rtn;
    while (*sym == "==" || *sym == "!="){
        var1 = var_temp;
        string op = *sym;
        sym++;
        RelExp(elements);
        var2 = rtn;
        var_temp = var2;
        int new_reg = memory.size();
        if (op == "=="){
            output.push_back("    %x" + to_string(new_reg) + " = icmp eq i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %x" + to_string(new_reg) + " = icmp eq i32 " + var1 + ", " + var2<<endl;
        }
        if (op == "!="){
            output.push_back("    %x" + to_string(new_reg) + " = icmp ne i32 " + var1 + ", " + var2 + "\n");
            //cout<<"    %x" + to_string(new_reg) + " = icmp ne i32 " + var1 + ", " + var2<<endl;
        }
        memory.push(var1 + op + var2);
        rtn = "%x" + to_string(new_reg);
        /*new_reg = memory.size();
        output.push_back("    %x" + to_string(new_reg) + " = zext i1 " + rtn + " to i32\n");
        //cout<<"    %x" + to_string(new_reg) + " = zext i1 " + rtn + " to i32"<<endl;
        memory.push(rtn + "to i32");
        rtn = "%x" + to_string(new_reg);*/
    }
}

void LAndExp(vector<element> &elements, int upper_block_next_exec){
    int block_next = memory.size();
    memory.push("block_next");
    EqExp(elements);
    output.push_back("    br i1 " + rtn +", label %x" + to_string(block_next) + ", label %x" + to_string(upper_block_next_exec) + "\n");
    //cout<<"    br i1 " + rtn +", label %x" + to_string(block_next) + ", label %x" + to_string(upper_block_next_exec)<<endl;
    while (*sym == "&&"){
        sym++;
        block_next = memory.size();
        memory.push("block_next");
        output.push_back("\n" + to_string(block_next) + ":\n");
        //cout<<"\n" + to_string(block_next) + ":"<<endl;
        EqExp(elements);
        output.push_back("    br i1 " + rtn +", label %x" + to_string(block_next) + ", label %x" + to_string(upper_block_next_exec) + "\n");
        //cout<<"    br i1 " + rtn +", label %x" + to_string(block_next) + ", label %x" + to_string(upper_block_next_exec)<<endl;
    }
    output.push_back("\n" + to_string(block_next) + ":\n");
    //cout<<"\n" + to_string(block_next) + ":"<<endl;
}

void LOrExp(vector<element> &elements, int block_exec, int block_false_exec){
    int block_next = memory.size();
    memory.push("block_next");
    LAndExp(elements, block_next);
    output.push_back("    br i1 " + rtn +", label %x" + to_string(block_exec) + ", label %x" + to_string(block_next) + "\n");
    //cout<<"    br i1 " + rtn +", label %x" + to_string(block_exec) + ", label %x" + to_string(block_next)<<endl;
    while (*sym == "||"){
        sym++;
        block_next = memory.size();
        memory.push("block_next");
        output.push_back("\n" + to_string(block_next) + ":\n");
        //cout<<"\n" + to_string(block_next) + ":"<<endl;
        LAndExp(elements, block_next);
        output.push_back("    br i1 " + rtn +", label %x" + to_string(block_exec) + ", label %x" + to_string(block_next) + "\n");
        //cout<<"    br i1 " + rtn +", label %x" + to_string(block_exec) + ", label %x" + to_string(block_next)<<endl;
    }
    output.push_back("\n" + to_string(block_next) + ":\n");
    //cout<<"\n" + to_string(block_next) + ":"<<endl;
    output.push_back("    br i1 " + rtn +", label %x" + to_string(block_exec) + ", label %x" + to_string(block_false_exec) + "\n");
    //cout<<"    br i1 " + rtn +", label %x" + to_string(block_exec) + ", label %x" + to_string(block_false_exec)<<endl;
}

void Ident(){
    if (isIdent(*sym)){
        sym++;
    }
    else{
        exit(1);
    }
}
