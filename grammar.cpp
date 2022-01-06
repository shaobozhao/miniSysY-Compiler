#include <iostream>
#include <cstdio>
#include <vector>
#include <stack>
#include <algorithm>
#include "public.h"

using namespace std;

struct element{
    string name;
    bool isConst;
    string type;
    int dim = 0;
    int size = 1;
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
void ConstExp(vector<element> &elements);
void VarDecl(vector<element> &elements);
void VarDef(vector<element> &elements);
void InitVal(vector<element> &elements, element elem);
void FuncDef(vector<element> &elements);
void FuncType();
void Block(vector<element> &elements);
void BlockItem(vector<element> &elements);
void Stmt(vector<element> &elements);
void Exp(vector<element> &elements, bool isConst);
void Cond(vector<element> &elements, int &block_exec, int &block_false);
void LVal(vector<element> &elements);
void PrimaryExp(vector<element> &elements, bool isConst);
void UnaryExp(vector<element> &elements, bool isConst);
void UnaryOp();
void FuncRParams(vector<element> &elements, function func, string &params);
void MulExp(vector<element> &elements, bool isConst);
void AddExp(vector<element> &elements, bool isConst);
void RelExp(vector<element> &elements);
void EqExp(vector<element> &elements);
void LAndExp(vector<element> &elements, int &or_block_next);
void LOrExp(vector<element> &elements, int &block_exec, int &block_false);
void Ident();

int level;

bool is_element(vector<element> &elements, const string &str){
    auto elem_iter = find_if(elements.begin(), elements.end(), [str](element elem){
                                 if (elem.name == str && elem.level == level){
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
    elem.level = -1;
    for (int i = elements.size() - 1; i > -1; i--){
        if (elements[i].name == name){
            elem = elements[i];
            break;
        }
    }
    return elem;
}

const function getint = {.name = "getint", .type = "i32", .argc = 0, .argv = {}},
               getch = {.name = "getch", .type = "i32", .argc = 0, .argv = {}},
               getarray = {.name = "getarray", .type = "i32", .argc = 1, .argv = {"i32*"}},
               putint = {.name = "putint", .type = "void", .argc = 1, .argv = {"i32"}},
               putch = {.name = "putch", .type = "void", .argc = 1, .argv = {"i32"}},
               putarray = {.name = "putarray", .type = "void", .argc = 2, .argv = {"i32", "i32*"}},
               memset = {.name = "memset", .type = "void", .argc = 3, .argv = {"i32*", "i32", "i32"}};

const vector<function> miniSysY = {getint, getch, getarray, putint, putch, putarray, memset};

vector<function> functions;

stack<string> memory;
pair<string, string> rtn;

bool isIdent(const string &str){
    bool nondigit = (isalpha(str.at(0)) || str.at(0) == '_');
    for (int i = 1; i < str.length(); i++){
        if (!isalnum(str.at(i)) && str.at(i) != '_'){
            return false;
        }
    }
    bool reserved = str != "const" && str != "int" &&
                    str != "if" && str != "else" &&
                    str != "while" && str != "break" &&
                    str != "continue" && str != "return";
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
        if ((*call_iter).type == "i32"){
            call_line = "declare i32 @" + (*call_iter).name + "(";
        }
        else{
            call_line = "declare void @" + (*call_iter).name + "(";
        }
        for (int i = 0; i <(*call_iter).argc; i++){
            call_line += (*call_iter).argv[i];
            if (i < (*call_iter).argc - 1){
                call_line += ", ";
            }
        }
        call_line += ")\n";
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
    while (sym != syms.end()){
        if (*sym == "const" || (*sym == "int" && *(sym + 2) != "(")){
            Decl(elements);
        }
        else{
            FuncDef(elements);
        }
    }
}

void Decl(vector<element> &elements){
    if (*sym == "const"){
        ConstDecl(elements);
    }
    else if (*sym == "int"){
        VarDecl(elements);
    }
    else{
        exit(1);
    }
}

void ConstDecl(vector<element> &elements){
    if (*sym == "const"){
        sym++;
        BType();
        ConstDef(elements);
        while (*sym == ","){
            sym++;
            ConstDef(elements);
        }
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

void BType(){
    if (*sym == "int"){
        sym++;
    }
    else{
        exit(1);
    }
}

void ConstDef(vector<element> &elements){
    string name = *sym;
    Ident();
    if (is_element(elements, name)){
        exit(1);
    }
    element elem;
    elem.name = name;
    elem.isConst = true;
    if (*sym == "["){
        elem.type = "";
        while (*sym == "["){
            elem.type += "[";
            sym++;
            ConstExp(elements);
            elem.type += (rtn.first + " x ");
            if (*sym == "]"){
                elem.dim++;
                elem.size *= stoi(rtn.first, 0);
                sym++;
            }
            else{
                exit(1);
            }
        }
        elem.type += "i32" + string(elem.dim, ']');
    }
    else{
        elem.type = "i32";
    }
    elem.level = level;
    if (*sym == "="){
        sym++;
        if (elem.type != "i32"){
            if (elem.level == 0){
                output.push_back("@" + elem.name + " = dso_local constant ");
                //cout << "@" + elem.name + " = dso_local constant ";
            }
            else{
                elem.reg = "%x" + to_string(memory.size());
                output.push_back("    " + elem.reg + " = alloca " + elem.type + "\n");
                //cout << "    " + elem.reg + " = alloca " + elem.type << endl;
                memory.push("");
            }
        }
    }
    else{
        exit(1);
    }
    ConstInitVal(elements, elem);
    if (elem.type == "i32"){
        elem.reg = rtn.first;
    }
    else{
        if (elem.level == 0){
            output.push_back("\n");
            //cout << endl;
            elem.reg = "@" + elem.name;
        }
    }
    elements.push_back(elem);
}

void ConstInitVal(vector<element> &elements, element elem){
    if (elem.type == "i32"){
        ConstExp(elements);
        if (elem.name == "sub_elem"){
            output.push_back("i32 " + rtn.first);
            //cout << "i32 " + rtn.first;
        }
    }
    else{
        if (elem.level == 0){
            if (*sym == "{"){
                sym++;
                output.push_back(elem.type);
                //cout << elem.type;
                if (*sym != "}"){
                    output.push_back(" [");
                    //cout << " [";
                    int pos = elem.type.find("x") + 2, len = elem.type.size() - 1 - pos;
                    element sub_elem = {.name = "sub_elem", .isConst = true, .type = elem.type.substr(pos, len), .dim = elem.dim - 1, .level = elem.level, .reg = ""};
                    ConstInitVal(elements, sub_elem);
                    int i = 1, n = stoi(elem.type.substr(1, elem.type.find(" ") - 1), 0);
                    while (*sym == ","){
                        output.push_back(", ");
                        //cout << ", ";
                        sym++;
                        ConstInitVal(elements, sub_elem);
                        i++;
                    }
                    while (i < n){
                        output.push_back(", " + sub_elem.type);
                        //cout << ", ";
                        if (sub_elem.type == "i32"){
                            output.push_back(" 0");
                            //cout << " 0";
                        }
                        else{
                            output.push_back(" zeroinitializer");
                            //cout << " zeroinitializer";
                        }
                        i++;
                    }
                    if (i > n){
                        exit(1);
                    }
                    output.push_back("]");
                    //cout << "]";
                }
                else{
                    output.push_back(" zeroinitializer");
                    //cout << " zeroinitializer";
                }
                if (*sym == "}"){
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
            int new_reg = memory.size();
            output.push_back("    %x" + to_string(new_reg) + " = getelementptr " + elem.type + ", " + elem.type + "* " + elem.reg + ", i32 0");
            //cout << "    %x" + to_string(new_reg) + " = getelementptr " + elem.type + ", " + elem.type + "* " + elem.reg + ", i32 0";
            for (int i = 0; i < elem.dim; i++){
                output.push_back(", i32 0");
                //cout << ", i32 0";
            }
            output.push_back("\n");
            //cout << endl;
            memory.push(elem.reg);
            isFunc("memset");
            output.push_back("    call void @memset(i32* %x" + to_string(new_reg) + ", i32 0, i32 " + to_string(elem.size * 4) + ")\n");
            //cout << "    call void @memset(i32* %x" + to_string(new_reg) + ", i32 0, i32 " + to_string(elem.size * 4) + ")" <<endl;
            int dim = -1, index[elem.dim] = {0};
            if(*sym == "{"){
                dim++;
                sym++;
                while (dim >= 0){
                    if (*sym == "{"){
                        dim++;
                        sym++;
                    }
                    else if (*sym == ",")
                    {
                        index[dim]++;
                        sym++;
                    }
                    else if (*sym == "}"){
                        index[dim] = 0;
                        dim--;
                        sym++;
                    }
                    else{
                        new_reg = memory.size();
                        output.push_back("    %x" + to_string(new_reg) + " = getelementptr " + elem.type + ", " + elem.type + "* " + elem.reg + ", i32 0");
                        //cout << "    %x" + to_string(new_reg) + " = getelementptr " + elem.type + ", " + elem.type + "* " + elem.reg + ", i32 0";
                        for (int i = 0; i < elem.dim; i++){
                            output.push_back(", i32 " + to_string(index[i]));
                            //cout << ", i32 " + to_string(index[i]);
                        }
                        output.push_back("\n");
                        //cout << endl;
                        memory.push(elem.reg);
                        /*int ptr_reg = new_reg;
                        new_reg = memory.size();
                        output.push_back("    %x" + to_string(new_reg) + " = getelementptr i32, i32* %x" + to_string(ptr_reg) + ", i32 0\n");
                        //cout << "    %x" + to_string(new_reg) + " = getelementptr i32, i32* %x" + to_string(ptr_reg) + ", i32 0" << endl;
                        memory.push("%x" + to_string(ptr_reg));*/
                        ConstExp(elements);
                        output.push_back("    store i32 " + rtn.first + ", i32* %x" + to_string(new_reg) + "\n");
                        //cout << "    store i32 " + rtn.first + ", i32* %x" + to_string(new_reg) <<endl;
                    }
                }
            }
            else{
                exit(1);
            }
        }
    }
}

void ConstExp(vector<element> &elements){
    AddExp(elements, true);
}

void VarDecl(vector<element> &elements){
    BType();
    VarDef(elements);
    while (*sym == ","){
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
    if (is_element(elements, name)){
        exit(1);
    }
    element elem;
    elem.name = name;
    elem.isConst = false;
    if (*sym == "["){
        elem.type = "";
        while (*sym == "["){
            elem.type += "[";
            sym++;
            ConstExp(elements);
            elem.type += (rtn.first + " x ");
            if (*sym == "]"){
                elem.dim++;
                elem.size *= stoi(rtn.first, 0);
                sym++;
            }
            else{
                exit(1);
            }           
        }
        elem.type += "i32" + string(elem.dim, ']');
    }
    else{
        elem.type = "i32";
    }
    elem.level = level;
    if (elem.level == 0){
        elem.reg = "@" + elem.name;
        output.push_back(elem.reg + " = dso_local global ");
        //cout << elem.reg + " = dso_local global ";
        if (elem.type == "i32"){
            output.push_back("i32 ");
            //cout <<"i32 ";
        }
    }
    else{
        elem.reg = "%x" + to_string(memory.size());
        output.push_back("    " + elem.reg + " = alloca ");
        //cout << "    " + elem.reg + " = alloca ";
        if (elem.type == "i32"){
            output.push_back("i32\n");
            //cout << "i32" << endl;
        }
        else{
            output.push_back(elem.type + "\n");
            //cout << elem.type << endl;
        }
        
        memory.push("");
    }
    if (*sym == "="){
        sym++;
        InitVal(elements, elem);
        if (elem.level == 0){
            output.push_back("\n");
            //cout << endl;
        }
    }
    else{
        if (elem.level == 0){
            if (elem.type == "i32"){
                output.push_back("0\n");
                //cout << 0 << endl;
            }
            else{
                output.push_back(elem.type + " zeroinitializer\n");
                //cout << elem.type + " zeroinitializer" << endl;
            }
        }
    }
    elements.push_back(elem);
}

void InitVal(vector<element> &elements, element elem){
    if (elem.level == 0){
        if (elem.type == "i32"){
            Exp(elements, true);
            if (elem.name != "sub_elem"){
                output.push_back(rtn.first);
                //cout << rtn.first;
            }
            else{
                output.push_back("i32 " + rtn.first);
                //cout << "i32 " + rtn.first;
            }
        }
        else{
            if (*sym == "{"){
                sym++;
                output.push_back(elem.type);
                //cout << elem.type;
                if (*sym != "}"){
                    output.push_back(" [");
                    //cout << " [";
                    int pos = elem.type.find("x") + 2, len = elem.type.size() - 1 - pos;
                    element sub_elem = {.name = "sub_elem", .isConst = elem.isConst, .type = elem.type.substr(pos, len), .dim = elem.dim - 1, .level = elem.level, .reg = ""};
                    InitVal(elements, sub_elem);
                    int i = 1, n = stoi(elem.type.substr(1, elem.type.find(" ") - 1), 0);
                    while (*sym == ","){
                        output.push_back(", ");
                        //cout << ", ";
                        sym++;
                        InitVal(elements, sub_elem);
                        i++;
                    }
                    while (i < n){
                        output.push_back(", " + sub_elem.type);
                        //cout << ", " + sub_elem.type;
                        if (sub_elem.type == "i32"){
                            output.push_back(" 0");
                            //cout << " 0";
                        }
                        else{
                            output.push_back(" zeroinitializer");
                            //cout << " zeroinitializer";
                        }
                        i++;
                    }
                    if (i > n){
                        exit(1);
                    }
                    output.push_back("]");
                    //cout << "]";
                }
                else{
                    output.push_back(" zeroinitializer");
                    //cout << " zeroinitializer";
                }
                if (*sym == "}"){
                    sym++;
                }
                else{
                    exit(1);
                }   
            }
            else
            {
                exit(1);
            }
        }
    }
    else{
        if (elem.type == "i32"){
            Exp(elements, false);
            output.push_back("    store i32 " + rtn.first + ", i32* " + elem.reg + "\n");
            //cout << "    store i32 " + rtn.first + ", i32* " + elem.reg << endl;
        }
        else{
            int new_reg = memory.size();
            output.push_back("    %x" + to_string(new_reg) + " = getelementptr " + elem.type + ", " + elem.type + "* " + elem.reg + ", i32 0");
            //cout << "    %x" + to_string(new_reg) + " = getelementptr " + elem.type + ", " + elem.type + "* " + elem.reg + ", i32 0";
            for (int i = 0; i < elem.dim; i++){
                output.push_back(", i32 0");
                //cout << ", i32 0";
            }
            output.push_back("\n");
            //cout << endl;
            memory.push(elem.reg);
            isFunc("memset");
            output.push_back("    call void @memset(i32* %x" + to_string(new_reg) + ", i32 0, i32 " + to_string(elem.size * 4) + ")\n");
            //cout << "    call void @memset(i32* %x" + to_string(new_reg) + ", i32 0, i32 " + to_string(elem.size * 4) + ")" <<endl;
            int dim = -1, index[elem.dim] = {0};
            if(*sym == "{"){
                dim++;
                sym++;
                while (dim >= 0){
                    if (*sym == "{"){
                        dim++;
                        sym++;
                    }
                    else if (*sym == ",")
                    {
                        index[dim]++;
                        sym++;
                    }
                    else if (*sym == "}"){
                        index[dim] = 0;
                        dim--;
                        sym++;
                    }
                    else{
                        new_reg = memory.size();
                        output.push_back("    %x" + to_string(new_reg) + " = getelementptr " + elem.type + ", " + elem.type + "* " + elem.reg + ", i32 0");
                        //cout << "    %x" + to_string(new_reg) + " = getelementptr " + elem.type + ", " + elem.type + "* " + elem.reg + ", i32 0";
                        for (int i = 0; i < elem.dim; i++){
                            output.push_back(", i32 " + to_string(index[i]));
                            //cout << ", i32 " + to_string(index[i]);
                        }
                        output.push_back("\n");
                        //cout << endl;
                        memory.push(elem.reg);
                        /*int ptr_reg = new_reg;
                        new_reg = memory.size();
                        output.push_back("    %x" + to_string(new_reg) + " = getelementptr i32, i32* %x" + to_string(ptr_reg) + ", i32 0\n");
                        //cout << "    %x" + to_string(new_reg) + " = getelementptr i32, i32* %x" + to_string(ptr_reg) + ", i32 0" << endl;
                        memory.push("%x" + to_string(ptr_reg));*/
                        Exp(elements, false);
                        output.push_back("    store i32 " + rtn.first + ", i32* %x" + to_string(new_reg) + "\n");
                        //cout << "    store i32 " + rtn.first + ", i32* %x" + to_string(new_reg) <<endl;
                    }
                }
            }
            else{
                exit(1);
            }
        }
    }
}

void FuncDef(vector<element> &elements){
    FuncType();
    Ident();
    output.push_back("@main");
    //cout << "@main";
    memory.push("main");
    if (*sym == "("){
        output.push_back("(");
        //cout << "(";
        sym++;
        if (*sym == ")"){
            output.push_back(")");
            //cout << ")";
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
        //cout << "{" << endl;
        Block(elements);
        output.push_back("}\n");
        //cout << "}" << endl;
    }
}

void FuncType(){
    if (*sym == "int"){
        output.push_back("define dso_local i32 ");
        //cout << "define dso_local i32 ";
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
        while (*sym != "}"){
            BlockItem(sub_elements);
            if (sym == syms.end()){
                exit(1);
            }
        }
        level--;
        sym++;
    }
    else{
        exit(1);
    }
}

void BlockItem(vector<element> &elements){
    if (*sym == "const" || *sym == "int"){
        Decl(elements);
    }
    else{
        Stmt(elements);
    }
}

void Stmt(vector<element> &elements){
    if (isIdent(*sym) && !isFunc(*sym) && *(sym + 1) == "="){
        LVal(elements);
        element elem = get_elem_by_name(elements, *sym);
        if (elem.isConst){
            exit(1);
        }
        string reg;
        if (elem.type == "i32"){
            reg = elem.reg;
            sym++;
        }
        else{
            sym++;
            int new_reg = memory.size();
            output.push_back("    %x" + to_string(new_reg) + " = getelementptr " + elem.type + ", " + elem.type + "* " + elem.reg + ", i32 0");
            //cout << "    %x" + to_string(new_reg) + " = getelementptr " + elem.type + ", " + elem.type + "* " + elem.reg + ", i32 0";
            int dim = 0;
            while (*sym == "["){
                sym++;
                Exp(elements, false);
                output.push_back(", i32 " + rtn.first);
                //cout << ", i32 " + rtn.first;
                if (*sym == "]"){
                    dim++;
                    sym++;
                }
                else{
                    exit(1);
                }                
            }
            if (dim != elem.dim){
                exit(1);
            }
            output.push_back("\n");
            //cout << endl;
            memory.push(elem.reg);
            /*int ptr_reg = new_reg;
            new_reg = memory.size();
            output.push_back("    %x" + to_string(new_reg) + " = getelementptr i32, i32* %x" + to_string(ptr_reg) + ", i32 0\n");
            //cout << "    %x" + to_string(new_reg) + " = getelementptr i32, i32* %x" + to_string(ptr_reg) + ", i32 0" << endl;
            memory.push("%x" + to_string(ptr_reg));*/
            reg = "%x" + to_string(new_reg);
        }        
        if (*sym == "="){
            sym++;
            Exp(elements, false);
            output.push_back("    store i32 " + rtn.first + ", i32* " + reg + "\n");
            //cout << "    store i32 " + rtn.first + ", i32* " + reg << endl;
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
            int block_true, block_false, block_out;
            Cond(elements, block_true, block_false);
            if (*sym == ")"){
                sym++;
                output.push_back("\nx" + to_string(block_true) + ":\n");
                //cout << "\nx" + to_string(block_true) + ":" << endl;
                Stmt(elements);
                block_out = memory.size();
                memory.push("block_out");
                output.push_back("    br label %x" + to_string(block_out) + "\n");
                //cout << "    br label %x" + to_string(block_false) << endl;
                output.push_back("\nx" + to_string(block_false) + ":\n");
                //cout << "\nx" + to_string(block_false) + ":" << endl;
                if (*sym == "else"){
                    sym++;
                    Stmt(elements);
                }
                output.push_back("    br label %x" + to_string(block_out) + "\n");
                //cout << "    br label %x" + to_string(block_out) << endl;
                output.push_back("\nx" + to_string(block_out) + ":\n");
                //cout << "\nx" + to_string(block_out) + ":" << endl;
            }
            else{
                exit(1);
            }
        }
        else{
            exit(1);
        }
    }
    else if (*sym == "while"){
        sym++;
        if (*sym == "("){
            sym++;
            int block_cond, block_true, block_false;
            block_cond = memory.size();
            memory.push("block_cond");
            output.push_back("    br label %x" + to_string(block_cond) + "\n");
            //cout << "    br label %x" + to_string(block_cond) << endl;
            output.push_back("\nx" + to_string(block_cond) + ":\n");
            //cout << "\nx" + to_string(block_cond) + ":" << endl;
            Cond(elements, block_true, block_false);
            if (*sym == ")"){
                sym++;
                int br_size = output.size(), cnt_size = output.size();
                output.push_back("\nx" + to_string(block_true) + ":\n");
                //cout << "\nx" + to_string(block_true) + ":" << endl;
                Stmt(elements);
                for (int i = br_size; i < output.size(); i++){
                    if (output[i] == "continue_record"){
                        output[i] = "    br label %x" + to_string(block_cond) + "\n";
                        //cout << "    br label %x" + to_string(block_cond) << endl;
                    }
                }
                output.push_back("    br label %x" + to_string(block_cond) + "\n");
                for (int i = cnt_size; i < output.size(); i++){
                    if (output[i] == "break_record"){
                        output[i] = "    br label %x" + to_string(block_false) + "\n";
                        //cout << "    br label %x" + to_string(block_false) << endl;
                    }
                }
                //cout << "    br label %x" + to_string(block_cond) << endl;
                output.push_back("\nx" + to_string(block_false) + ":\n");
                //cout << "\nx" + to_string(block_false) + ":" << endl;
            }
            else{
                exit(1);
            }
        }
        else{
            exit(1);
        }
    }
    else if (*sym == "break"){
        output.push_back("break_record");
        sym++;
        if (*sym == ";"){
            sym++;
        }
        else{
            exit(1);
        }
    }
    else if (*sym == "continue"){
        output.push_back("continue_record");
        sym++;
        if (*sym == ";"){
            sym++;
        }
        else{
            exit(1);
        }
    }
    else if (*sym == "return"){
        sym++;
        Exp(elements, false);
        if (*sym == ";"){
            output.push_back("    ret i32 " + rtn.first + "\n");
            //cout << "    ret i32 " + rtn.first << endl;
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
        else{
            exit(1);
        }
    }
}

void Exp(vector<element> &elements, bool isConst){
    AddExp(elements, isConst);
}

void Cond(vector<element> &elements, int &block_exec, int &block_false){
    LOrExp(elements, block_exec, block_false);
}

void LVal(vector<element> &elements){
    if (!is_element(elements, *sym) && get_elem_by_name(elements, *sym).level < 0){
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
        rtn.first = *sym;
        rtn.second = "i32";
        sym++;
    }
    else if (isIdent(*sym)){
        LVal(elements);
        element elem = get_elem_by_name(elements, *sym);
        if (isConst){
            if (elem.isConst && elem.type == "i32"){
                rtn.first = elem.reg;
                rtn.second = "i32";
                sym++;
            }
            else{
                exit(1);
            }
        }
        else{
            if (elem.isConst){
                if (elem.type == "i32"){
                    rtn.first = elem.reg;
                    rtn.second = "i32";
                    sym++;
                }
                else{
                    sym++;
                    int new_reg = memory.size();
                    output.push_back("    %x" + to_string(new_reg) + " = getelementptr " + elem.type + ", " + elem.type + "* " + elem.reg + ", i32 0");
                    //cout << "    %x" + to_string(new_reg) + " = getelementptr " + elem.type + ", " + elem.type + "* " + elem.reg + ", i32 0";
                    int dim = 0;
                    while (*sym == "["){
                        sym++;
                        Exp(elements, false);
                        output.push_back(", i32 " + rtn.first);
                        //cout << ", i32 " + rtn.first;
                        if (*sym == "]"){
                            dim++;
                            sym++;
                        }
                        else{
                            exit(1);
                        }
                    }
                    if (dim != elem.dim){
                        exit(1);
                    }
                    output.push_back("\n");
                    //cout << endl;
                    memory.push(elem.reg);
                    int ptr_reg = new_reg;
                    new_reg = memory.size();
                    output.push_back("    %x" + to_string(new_reg) + " = load i32, i32* %x" + to_string(ptr_reg) + "\n");
                    //cout << "    %x" + to_string(new_reg) + " = load i32, i32* %x" + to_string(ptr_reg) << endl;
                    memory.push("%x" + to_string(ptr_reg));
                    rtn.first = "%x" + to_string(new_reg);
                    rtn.second = "i32";
                }
            }
            else{
                if (elem.type == "i32"){
                    int new_reg = memory.size();
                    output.push_back("    %x" + to_string(new_reg) + " = load i32, i32* " + elem.reg + "\n");
                    //cout << "    %x" + to_string(new_reg) + " = load i32, i32* " + elem.reg << endl;
                    memory.push(elem.reg);
                    rtn.first = "%x" + to_string(new_reg);
                    rtn.second = "i32";
                    sym++;
                }
                else{
                    sym++;
                    int new_reg = memory.size();
                    output.push_back("    %x" + to_string(new_reg) + " = getelementptr " + elem.type + ", " + elem.type + "* " + elem.reg + ", i32 0");
                    //cout << "    %x" + to_string(new_reg) + " = getelementptr " + elem.type + ", " + elem.type + "* " + elem.reg + ", i32 0";
                    int dim = 0;
                    while (*sym == "["){
                        sym++;
                        Exp(elements, false);
                        output.push_back(", i32 " + rtn.first);
                        //cout << ", i32 " + rtn.first;
                        if (*sym == "]"){
                            dim++;
                            sym++;
                        }
                        else{
                            exit(1);
                        }
                    }
                    if (dim != elem.dim){
                        exit(1);
                    }
                    output.push_back("\n");
                    //cout << endl;
                    memory.push(elem.reg);
                    int ptr_reg = new_reg;
                    new_reg = memory.size();
                    output.push_back("    %x" + to_string(new_reg) + " = load i32, i32* %x" + to_string(ptr_reg) + "\n");
                    //cout << "    %x" + to_string(new_reg) + " = load i32, i32* %x" + to_string(ptr_reg) << endl;
                    memory.push("%x" + to_string(ptr_reg));
                    rtn.first = "%x" + to_string(new_reg);
                    rtn.second = "i32";
                }
            }
        }
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
                FuncRParams(elements, func, params);
                if (*sym == ")"){
                    if (func.type == "i32"){
                        int new_reg = memory.size();
                        output.push_back("    %x" + to_string(new_reg) + " = call i32 @" + func.name + "(" + params + ")\n");
                        //cout << "    %x" + to_string(new_reg) + " = call i32 @" + func.name + "(" + params + ")" << endl;
                        memory.push(func.name + "(" + params + ")");
                        rtn.first = "%x" + to_string(new_reg);
                        rtn.second = "i32";
                    }
                    else{
                        output.push_back("    call void @" + func.name + "(" + params + ")\n");
                        //cout << "    call void @" + func.name + "(" + params + ")" << endl;
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
            if (isConst){
                int rtn_value = -stoi(rtn.first, 0);
                rtn.first = to_string(rtn_value);
                rtn.second = "i32";
            }
            else{
                int new_reg = memory.size();
                output.push_back("    %x" + to_string(new_reg) + " = sub i32 0, " + rtn.first + "\n");
                //cout << "    %x" + to_string(new_reg) + " = sub i32 0, " + rtn.first << endl;
                memory.push("-" + rtn.first);
                rtn.first = "%x" + to_string(new_reg);
                rtn.second = "i32";
            }
        }
        if (inverse){
            int new_reg = memory.size();
            output.push_back("    %x" + to_string(new_reg) + " = icmp eq i32 " + rtn.first + ", 0\n");
            //cout << "    %x" + to_string(new_reg) + " = icmp eq i32 " + rtn.first + ", 0" << endl;
            memory.push("!" + rtn.first);
            rtn.first = "%x" + to_string(new_reg);
            rtn.second = "i1";
            new_reg = memory.size();
            output.push_back("    %x" + to_string(new_reg) + " = zext i1 " + rtn.first + " to i32\n");
            //cout << "    %x" + to_string(new_reg) + " = zext i1 " + rtn.first + " to i32" << endl;
            memory.push(rtn.first + "to i32");
            rtn.first = "%x" + to_string(new_reg);
            rtn.second = "i32";
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
void FuncRParams(vector<element> &elements, function func, string &params){
    int argc = 0;
    while (*sym != ")"){
        Exp(elements, false);
        if (func.argv[argc] == rtn.second){
            params += (func.argv[argc] + " " + rtn.first);
            argc++;
        }
        else{
            exit(1);
        }
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
    while (*sym == "*" || *sym == "/" || *sym == "%"){
        var1 = rtn.first;
        string op = *sym;
        sym++;
        UnaryExp(elements, isConst);
        var2 = rtn.first;
        if (isConst){
            int var1_value = stoi(var1, 0), var2_value = stoi(var2, 0), rtn_value;
            if (op == "*"){
                rtn_value = var1_value * var2_value;
            }
            if (op == "/"){
                rtn_value = var1_value / var2_value;
            }
            if (op == "%"){
                rtn_value = var1_value % var2_value;
            }
            rtn.first = to_string(rtn_value);
            rtn.second = "i32";
        }
        else{
            int new_reg = memory.size();
            if (op == "*"){
                output.push_back("    %x" + to_string(new_reg) + " = mul i32 " + var1 + ", " + var2 + "\n");
                //cout << "    %x" + to_string(new_reg) + " = mul i32 " + var1 + ", " + var2 << endl;
            }
            if (op == "/"){
                output.push_back("    %x" + to_string(new_reg) + " = sdiv i32 " + var1 + ", " + var2 + "\n");
                //cout << "    %x" + to_string(new_reg) + " = sdiv i32 " + var1 + ", " + var2 << endl;
            }
            if (op == "%"){
                output.push_back("    %x" + to_string(new_reg) + " = srem i32 " + var1 + ", " + var2 + "\n");
                //cout << "    %x" + to_string(new_reg) + " = srem i32 " + var1 + ", " + var2 << endl;
            }
            memory.push(var1 + op + var2);
            rtn.first = "%x" + to_string(new_reg);
            rtn.second = "i32";
        }
    }
}

void AddExp(vector<element> &elements, bool isConst){
    MulExp(elements, isConst);
    string var1, var2;
    while (*sym == "+" || *sym == "-"){
        var1 = rtn.first;
        string op = *sym;
        sym++;
        MulExp(elements, isConst);
        var2 = rtn.first;
        if (isConst){
            int var1_value = stoi(var1, 0), var2_value = stoi(var2, 0), rtn_value;
            if (op == "+"){
                rtn_value = var1_value + var2_value;
            }
            if (op == "-"){
                rtn_value = var1_value - var2_value;
            }
            rtn.first = to_string(rtn_value);
            rtn.second = "i32";
        }
        else
        {
            int new_reg = memory.size();
            if (op == "+"){
                output.push_back("    %x" + to_string(new_reg) + " = add i32 " + var1 + ", " + var2 + "\n");
                //cout << "    %x" + to_string(new_reg) + " = add i32 " + var1 + ", " + var2 << endl;
            }
            if (op == "-"){
                output.push_back("    %x" + to_string(new_reg) + " = sub i32 " + var1 + ", " + var2 + "\n");
                //cout << "    %x" + to_string(new_reg) + " = sub i32 " + var1 + ", " + var2 << endl;
            }
            memory.push(var1 + op + var2);
            rtn.first = "%x" + to_string(new_reg);
            rtn.second = "i32";
        }
    }
}

void RelExp(vector<element> &elements){
    AddExp(elements, false);
    string var1, var2, var_temp = rtn.first;
    while (*sym == "<" || *sym == ">" || *sym == "<=" || *sym == ">="){
        var1 = var_temp;
        string op = *sym;
        sym++;
        AddExp(elements, false);
        var2 = rtn.first;
        var_temp = var2;
        int new_reg = memory.size();
        if (op == "<"){
            output.push_back("    %x" + to_string(new_reg) + " = icmp slt i32 " + var1 + ", " + var2 + "\n");
            //cout << "    %x" + to_string(new_reg) + " = icmp slt i32 " + var1 + ", " + var2 << endl;
        }
        if (op == ">"){
            output.push_back("    %x" + to_string(new_reg) + " = icmp sgt i32 " + var1 + ", " + var2 + "\n");
            //cout << "    %x" + to_string(new_reg) + " = icmp sgt i32 " + var1 + ", " + var2 << endl;
        }
        if (op == "<="){
            output.push_back("    %x" + to_string(new_reg) + " = icmp sle i32 " + var1 + ", " + var2 + "\n");
            //cout << "    %x" + to_string(new_reg) + " = icmp sle i32 " + var1 + ", " + var2 << endl;
        }
        if (op == ">="){
            output.push_back("    %x" + to_string(new_reg) + " = icmp sge i32 " + var1 + ", " + var2 + "\n");
            //cout << "    %x" + to_string(new_reg) + " = icmp sge i32 " + var1 + ", " + var2 << endl;
        }
        memory.push(var1 + op + var2);
        rtn.first = "%x" + to_string(new_reg);
        rtn.second = "i1";

        new_reg = memory.size();
        output.push_back("    %x" + to_string(new_reg) + " = zext i1 " + rtn.first + " to i32\n");
        //cout << "    %x" + to_string(new_reg) + " = zext i1 " + rtn.first + " to i32" << endl;
        memory.push(rtn.first + "to i32");
        rtn.first = "%x" + to_string(new_reg);
        rtn.second = "i32";
    }
}

void EqExp(vector<element> &elements){
    RelExp(elements);
    string var1, var2, var_temp = rtn.first;
    while (*sym == "==" || *sym == "!="){
        var1 = var_temp;
        string op = *sym;
        sym++;
        RelExp(elements);
        var2 = rtn.first;
        var_temp = var2;
        int new_reg = memory.size();
        if (op == "=="){
            output.push_back("    %x" + to_string(new_reg) + " = icmp eq i32 " + var1 + ", " + var2 + "\n");
            //cout << "    %x" + to_string(new_reg) + " = icmp eq i32 " + var1 + ", " + var2 << endl;
        }
        if (op == "!="){
            output.push_back("    %x" + to_string(new_reg) + " = icmp ne i32 " + var1 + ", " + var2 + "\n");
            //cout << "    %x" + to_string(new_reg) + " = icmp ne i32 " + var1 + ", " + var2 << endl;
        }
        memory.push(var1 + op + var2);
        rtn.first = "%x" + to_string(new_reg);
        rtn.second = "i1";

        new_reg = memory.size();
        output.push_back("    %x" + to_string(new_reg) + " = zext i1 " + rtn.first + " to i32\n");
        //cout << "    %x" + to_string(new_reg) + " = zext i1 " + rtn.first + " to i32" << endl;
        memory.push(rtn.first + "to i32");
        rtn.first = "%x" + to_string(new_reg);
        rtn.second = "i32";
    }
    int new_reg = memory.size();
    output.push_back("    %x" + to_string(new_reg) + " = icmp ne i32 " + rtn.first + ", 0\n");
    //cout << "    %x" + to_string(new_reg) + " = icmp ne i32 " + rtn.first + ", 0" << endl;
    memory.push(rtn.first + "to i1");
    rtn.first = "%x" + to_string(new_reg);
    rtn.second = "i1";
}

void LAndExp(vector<element> &elements, int &or_block_next){
    EqExp(elements);
    int block_next = memory.size();
    memory.push("block_next");
    or_block_next = memory.size();
    memory.push("upper_block_next_exec");
    output.push_back("    br i1 " + rtn.first + ", label %x" + to_string(block_next) + ", label %x" + to_string(or_block_next) + "\n");
    //cout << "    br i1 " + rtn.first + ", label %x" + to_string(block_next) + ", label %x" + to_string(or_block_next) << endl;
    while (*sym == "&&"){
        sym++;
        output.push_back("\nx" + to_string(block_next) + ":\n");
        //cout << "\nx" + to_string(block_next) + ":" << endl;
        EqExp(elements);
        block_next = memory.size();
        memory.push("block_next");
        output.push_back("    br i1 " + rtn.first + ", label %x" + to_string(block_next) + ", label %x" + to_string(or_block_next) + "\n");
        //cout << "    br i1 " + rtn.first + ", label %x" + to_string(block_next) + ", label %x" + to_string(or_block_next) << endl;
    }
    output.push_back("\nx" + to_string(block_next) + ":\n");
    //cout << "\nx" + to_string(block_next) + ":" << endl;
}

void LOrExp(vector<element> &elements, int &block_true, int &block_false){
    int block_next;
    LAndExp(elements, block_next);
    int block_exec = memory.size();
    memory.push("block_exec");
    output.push_back("    br i1 " + rtn.first + ", label %x" + to_string(block_exec) + ", label %x" + to_string(block_next) + "\n");
    //cout << "    br i1 " + rtn.first + ", label %x" + to_string(block_exec) + ", label %x" + to_string(block_next) << endl;
    while (*sym == "||"){
        sym++;
        output.push_back("\nx" + to_string(block_next) + ":\n");
        //cout << "\nx" + to_string(block_next) + ":" << endl;
        LAndExp(elements, block_next);
        output.push_back("    br i1 " + rtn.first + ", label %x" + to_string(block_exec) + ", label %x" + to_string(block_next) + "\n");
        //cout << "    br i1 " + rtn.first + ", label %x" + to_string(block_exec) + ", label %x" + to_string(block_next) << endl;
    }
    output.push_back("\nx" + to_string(block_exec) + ":\n");
    //cout << "\nx" + to_string(block_exec) + ":" << endl;
    block_true = memory.size();
    memory.push("block_true");
    output.push_back("    br label %x" + to_string(block_true) + "\n");
    //cout << "    br label %x" + to_string(block_true) << endl;
    output.push_back("\nx" + to_string(block_next) + ":\n");
    //cout << "\nx" + to_string(block_next) + ":" << endl;
    block_false = memory.size();
    memory.push("block_false");
    output.push_back("    br label %x" + to_string(block_false) + "\n");
    //cout << "    br label %x" + to_string(block_false) << endl;
}

void Ident(){
    if (isIdent(*sym)){
        sym++;
    }
    else{
        exit(1);
    }
}
