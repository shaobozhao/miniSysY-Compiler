#include <iostream>
#include <cstdio>
#include <vector>
#include <stack>
#include <algorithm>
#include "public.h"

using namespace std;

struct symbol{
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
void Decl(vector<symbol> &symbols);
void ConstDecl(vector<symbol> &symbols);
void BType();
void ConstDef(vector<symbol> &symbols);
void ConstInitVal(vector<symbol> &symbols, symbol sym);
void ConstExp(vector<symbol> &symbols);
void VarDecl(vector<symbol> &symbols);
void VarDef(vector<symbol> &symbols);
void InitVal(vector<symbol> &symbols, symbol sym);
void FuncDef(vector<symbol> &symbols);
void FuncType();
void Block(vector<symbol> &symbols);
void BlockItem(vector<symbol> &symbols);
void Stmt(vector<symbol> &symbols);
void Exp(vector<symbol> &symbols, bool isConst);
void Cond(vector<symbol> &symbols, int &block_exec, int &block_false);
void LVal(vector<symbol> &symbols);
void PrimaryExp(vector<symbol> &symbols, bool isConst);
void UnaryExp(vector<symbol> &symbols, bool isConst);
void UnaryOp();
void FuncRParams(vector<symbol> &symbols, function func, string &params);
void MulExp(vector<symbol> &symbols, bool isConst);
void AddExp(vector<symbol> &symbols, bool isConst);
void RelExp(vector<symbol> &symbols);
void EqExp(vector<symbol> &symbols);
void LAndExp(vector<symbol> &symbols, int &or_block_next);
void LOrExp(vector<symbol> &symbols, int &block_exec, int &block_false);
void Ident();

int level;

bool is_symbol(vector<symbol> &symbols, const string &str){
    auto sym_iter = find_if(symbols.begin(), symbols.end(), [str](symbol sym){
                                 if (sym.name == str && sym.level == level){
                                     return true;
                                 }
                                 else{
                                     return false;
                                 }
                             });
    if (sym_iter != symbols.end()){
        return true;
    }
    else{
        return false;
    }
}

symbol get_sym_by_name(vector<symbol> &symbols, string name){
    symbol sym;
    sym.level = -1;
    for (int i = symbols.size() - 1; i > -1; i--){
        if (symbols[i].name == name){
            sym = symbols[i];
            break;
        }
    }
    return sym;
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
    bool reserved = str != "const" && str != "int" && str != "void" &&
                    str != "if" && str != "else" && str != "while" &&
                    str != "break" && str != "continue" && str != "return";
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
    vector<symbol> symbols;
    while (token_iter != tokens.end()){
        if (*token_iter == "const" || (*token_iter == "int" && *(token_iter + 2) != "(")){
            Decl(symbols);
        }
        else{
            FuncDef(symbols);
        }
    }
}

void Decl(vector<symbol> &symbols){
    if (*token_iter == "const"){
        ConstDecl(symbols);
    }
    else if (*token_iter == "int"){
        VarDecl(symbols);
    }
    else{
        exit(1);
    }
}

void ConstDecl(vector<symbol> &symbols){
    if (*token_iter == "const"){
        token_iter++;
        BType();
        ConstDef(symbols);
        while (*token_iter == ","){
            token_iter++;
            ConstDef(symbols);
        }
        if (*token_iter == ";"){
            token_iter++;
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
    if (*token_iter == "int"){
        token_iter++;
    }
    else{
        exit(1);
    }
}

void ConstDef(vector<symbol> &symbols){
    string name = *token_iter;
    Ident();
    if (is_symbol(symbols, name)){
        exit(1);
    }
    symbol sym;
    sym.name = name;
    sym.isConst = true;
    if (*token_iter == "["){
        sym.type = "";
        while (*token_iter == "["){
            sym.type += "[";
            token_iter++;
            ConstExp(symbols);
            sym.type += (rtn.first + " x ");
            if (*token_iter == "]"){
                sym.dim++;
                sym.size *= stoi(rtn.first, 0);
                token_iter++;
            }
            else{
                exit(1);
            }
        }
        sym.type += "i32" + string(sym.dim, ']');
    }
    else{
        sym.type = "i32";
    }
    sym.level = level;
    if (*token_iter == "="){
        token_iter++;
        if (sym.type != "i32"){
            if (sym.level == 0){
                output.push_back("@" + sym.name + " = dso_local constant ");
                //cout << "@" + sym.name + " = dso_local constant ";
            }
            else{
                sym.reg = "%x" + to_string(memory.size());
                output.push_back("    " + sym.reg + " = alloca " + sym.type + "\n");
                //cout << "    " + sym.reg + " = alloca " + sym.type << endl;
                memory.push("");
            }
        }
    }
    else{
        exit(1);
    }
    ConstInitVal(symbols, sym);
    if (sym.type == "i32"){
        sym.reg = rtn.first;
    }
    else{
        if (sym.level == 0){
            output.push_back("\n");
            //cout << endl;
            sym.reg = "@" + sym.name;
        }
    }
    symbols.push_back(sym);
}

void ConstInitVal(vector<symbol> &symbols, symbol sym){
    if (sym.type == "i32"){
        ConstExp(symbols);
        if (sym.name == "sub_sym"){
            output.push_back("i32 " + rtn.first);
            //cout << "i32 " + rtn.first;
        }
    }
    else{
        if (sym.level == 0){
            if (*token_iter == "{"){
                token_iter++;
                output.push_back(sym.type);
                //cout << sym.type;
                if (*token_iter != "}"){
                    output.push_back(" [");
                    //cout << " [";
                    int pos = sym.type.find("x") + 2, len = sym.type.size() - 1 - pos;
                    symbol sub_sym = {.name = "sub_sym", .isConst = true, .type = sym.type.substr(pos, len), .dim = sym.dim - 1, .level = sym.level, .reg = ""};
                    ConstInitVal(symbols, sub_sym);
                    int i = 1, n = stoi(sym.type.substr(1, sym.type.find(" ") - 1), 0);
                    while (*token_iter == ","){
                        output.push_back(", ");
                        //cout << ", ";
                        token_iter++;
                        ConstInitVal(symbols, sub_sym);
                        i++;
                    }
                    while (i < n){
                        output.push_back(", " + sub_sym.type);
                        //cout << ", ";
                        if (sub_sym.type == "i32"){
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
                if (*token_iter == "}"){
                    token_iter++;
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
            output.push_back("    %x" + to_string(new_reg) + " = getelementptr " + sym.type + ", " + sym.type + "* " + sym.reg + ", i32 0");
            //cout << "    %x" + to_string(new_reg) + " = getelementptr " + sym.type + ", " + sym.type + "* " + sym.reg + ", i32 0";
            for (int i = 0; i < sym.dim; i++){
                output.push_back(", i32 0");
                //cout << ", i32 0";
            }
            output.push_back("\n");
            //cout << endl;
            memory.push(sym.reg);
            isFunc("memset");
            output.push_back("    call void @memset(i32* %x" + to_string(new_reg) + ", i32 0, i32 " + to_string(sym.size * 4) + ")\n");
            //cout << "    call void @memset(i32* %x" + to_string(new_reg) + ", i32 0, i32 " + to_string(sym.size * 4) + ")" <<endl;
            int dim = -1, index[sym.dim] = {0};
            if(*token_iter == "{"){
                dim++;
                token_iter++;
                while (dim >= 0){
                    if (*token_iter == "{"){
                        dim++;
                        token_iter++;
                    }
                    else if (*token_iter == ",")
                    {
                        index[dim]++;
                        token_iter++;
                    }
                    else if (*token_iter == "}"){
                        index[dim] = 0;
                        dim--;
                        token_iter++;
                    }
                    else{
                        new_reg = memory.size();
                        output.push_back("    %x" + to_string(new_reg) + " = getelementptr " + sym.type + ", " + sym.type + "* " + sym.reg + ", i32 0");
                        //cout << "    %x" + to_string(new_reg) + " = getelementptr " + sym.type + ", " + sym.type + "* " + sym.reg + ", i32 0";
                        for (int i = 0; i < sym.dim; i++){
                            output.push_back(", i32 " + to_string(index[i]));
                            //cout << ", i32 " + to_string(index[i]);
                        }
                        output.push_back("\n");
                        //cout << endl;
                        memory.push(sym.reg);
                        ConstExp(symbols);
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

void ConstExp(vector<symbol> &symbols){
    AddExp(symbols, true);
}

void VarDecl(vector<symbol> &symbols){
    BType();
    VarDef(symbols);
    while (*token_iter == ","){
        token_iter++;
        VarDef(symbols);
    }
    if (*token_iter == ";"){
        token_iter++;
    }
    else{
        exit(1);
    }
}

void VarDef(vector<symbol> &symbols){
    string name = *token_iter;
    Ident();
    if (is_symbol(symbols, name)){
        exit(1);
    }
    symbol sym;
    sym.name = name;
    sym.isConst = false;
    if (*token_iter == "["){
        sym.type = "";
        while (*token_iter == "["){
            sym.type += "[";
            token_iter++;
            ConstExp(symbols);
            sym.type += (rtn.first + " x ");
            if (*token_iter == "]"){
                sym.dim++;
                sym.size *= stoi(rtn.first, 0);
                token_iter++;
            }
            else{
                exit(1);
            }           
        }
        sym.type += "i32" + string(sym.dim, ']');
    }
    else{
        sym.type = "i32";
    }
    sym.level = level;
    if (sym.level == 0){
        sym.reg = "@" + sym.name;
        output.push_back(sym.reg + " = dso_local global ");
        //cout << sym.reg + " = dso_local global ";
        if (sym.type == "i32"){
            output.push_back("i32 ");
            //cout <<"i32 ";
        }
    }
    else{
        sym.reg = "%x" + to_string(memory.size());
        output.push_back("    " + sym.reg + " = alloca ");
        //cout << "    " + sym.reg + " = alloca ";
        if (sym.type == "i32"){
            output.push_back("i32\n");
            //cout << "i32" << endl;
        }
        else{
            output.push_back(sym.type + "\n");
            //cout << sym.type << endl;
        }
        
        memory.push("");
    }
    if (*token_iter == "="){
        token_iter++;
        InitVal(symbols, sym);
        if (sym.level == 0){
            output.push_back("\n");
            //cout << endl;
        }
    }
    else{
        if (sym.level == 0){
            if (sym.type == "i32"){
                output.push_back("0\n");
                //cout << 0 << endl;
            }
            else{
                output.push_back(sym.type + " zeroinitializer\n");
                //cout << sym.type + " zeroinitializer" << endl;
            }
        }
    }
    symbols.push_back(sym);
}

void InitVal(vector<symbol> &symbols, symbol sym){
    if (sym.level == 0){
        if (sym.type == "i32"){
            Exp(symbols, true);
            if (sym.name != "sub_sym"){
                output.push_back(rtn.first);
                //cout << rtn.first;
            }
            else{
                output.push_back("i32 " + rtn.first);
                //cout << "i32 " + rtn.first;
            }
        }
        else{
            if (*token_iter == "{"){
                token_iter++;
                output.push_back(sym.type);
                //cout << sym.type;
                if (*token_iter != "}"){
                    output.push_back(" [");
                    //cout << " [";
                    int pos = sym.type.find("x") + 2, len = sym.type.size() - 1 - pos;
                    symbol sub_sym = {.name = "sub_sym", .isConst = sym.isConst, .type = sym.type.substr(pos, len), .dim = sym.dim - 1, .level = sym.level, .reg = ""};
                    InitVal(symbols, sub_sym);
                    int i = 1, n = stoi(sym.type.substr(1, sym.type.find(" ") - 1), 0);
                    while (*token_iter == ","){
                        output.push_back(", ");
                        //cout << ", ";
                        token_iter++;
                        InitVal(symbols, sub_sym);
                        i++;
                    }
                    while (i < n){
                        output.push_back(", " + sub_sym.type);
                        //cout << ", " + sub_sym.type;
                        if (sub_sym.type == "i32"){
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
                if (*token_iter == "}"){
                    token_iter++;
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
        if (sym.type == "i32"){
            Exp(symbols, false);
            output.push_back("    store i32 " + rtn.first + ", i32* " + sym.reg + "\n");
            //cout << "    store i32 " + rtn.first + ", i32* " + sym.reg << endl;
        }
        else{
            int new_reg = memory.size();
            output.push_back("    %x" + to_string(new_reg) + " = getelementptr " + sym.type + ", " + sym.type + "* " + sym.reg + ", i32 0");
            //cout << "    %x" + to_string(new_reg) + " = getelementptr " + sym.type + ", " + sym.type + "* " + sym.reg + ", i32 0";
            for (int i = 0; i < sym.dim; i++){
                output.push_back(", i32 0");
                //cout << ", i32 0";
            }
            output.push_back("\n");
            //cout << endl;
            memory.push(sym.reg);
            isFunc("memset");
            output.push_back("    call void @memset(i32* %x" + to_string(new_reg) + ", i32 0, i32 " + to_string(sym.size * 4) + ")\n");
            //cout << "    call void @memset(i32* %x" + to_string(new_reg) + ", i32 0, i32 " + to_string(sym.size * 4) + ")" <<endl;
            int dim = -1, index[sym.dim] = {0};
            if(*token_iter == "{"){
                dim++;
                token_iter++;
                while (dim >= 0){
                    if (*token_iter == "{"){
                        dim++;
                        token_iter++;
                    }
                    else if (*token_iter == ",")
                    {
                        index[dim]++;
                        token_iter++;
                    }
                    else if (*token_iter == "}"){
                        index[dim] = 0;
                        dim--;
                        token_iter++;
                    }
                    else{
                        new_reg = memory.size();
                        output.push_back("    %x" + to_string(new_reg) + " = getelementptr " + sym.type + ", " + sym.type + "* " + sym.reg + ", i32 0");
                        //cout << "    %x" + to_string(new_reg) + " = getelementptr " + sym.type + ", " + sym.type + "* " + sym.reg + ", i32 0";
                        for (int i = 0; i < sym.dim; i++){
                            output.push_back(", i32 " + to_string(index[i]));
                            //cout << ", i32 " + to_string(index[i]);
                        }
                        output.push_back("\n");
                        //cout << endl;
                        memory.push(sym.reg);
                        Exp(symbols, false);
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

void FuncDef(vector<symbol> &symbols){
    FuncType();
    Ident();
    output.push_back("@main");
    //cout << "@main";
    memory.push("main");
    if (*token_iter == "("){
        output.push_back("(");
        //cout << "(";
        token_iter++;
        if (*token_iter == ")"){
            output.push_back(")");
            //cout << ")";
            token_iter++;
        }
        else{
            exit(1);
        }
    }
    else{
        exit(1);
    }
    if (*token_iter == "{"){
        output.push_back("{\n");
        //cout << "{" << endl;
        Block(symbols);
        output.push_back("}\n");
        //cout << "}" << endl;
    }
}

void FuncType(){
    if (*token_iter == "int"){
        output.push_back("define dso_local i32 ");
        //cout << "define dso_local i32 ";
        token_iter++;
    }
    else{
        exit(1);
    }
}

void Block(vector<symbol> &symbols){
    if (*token_iter == "{"){
        level++;
        vector<symbol> sub_symbols;
        sub_symbols.assign(symbols.begin(), symbols.end());
        token_iter++;
        while (*token_iter != "}"){
            BlockItem(sub_symbols);
            if (token_iter == tokens.end()){
                exit(1);
            }
        }
        level--;
        token_iter++;
    }
    else{
        exit(1);
    }
}

void BlockItem(vector<symbol> &symbols){
    if (*token_iter == "const" || *token_iter == "int"){
        Decl(symbols);
    }
    else{
        Stmt(symbols);
    }
}

void Stmt(vector<symbol> &symbols){
    if (isIdent(*token_iter) && !isFunc(*token_iter) && (*(token_iter + 1) == "=" || *(token_iter + 1) == "[")){
        LVal(symbols);
        symbol sym = get_sym_by_name(symbols, *token_iter);
        if (sym.isConst){
            exit(1);
        }
        string reg;
        if (sym.type == "i32"){
            reg = sym.reg;
            token_iter++;
        }
        else{
            token_iter++;
            int dim = 0;
            string index[sym.dim];
            while (*token_iter == "["){
                token_iter++;
                Exp(symbols, false);
                index[dim] = rtn.first;
                if (*token_iter == "]"){
                    dim++;
                    token_iter++;
                }
                else{
                    exit(1);
                }                
            }
            if (dim != sym.dim){
                exit(1);
            }
            int new_reg = memory.size();
            output.push_back("    %x" + to_string(new_reg) + " = getelementptr " + sym.type + ", " + sym.type + "* " + sym.reg + ", i32 0");
            //cout << "    %x" + to_string(new_reg) + " = getelementptr " + sym.type + ", " + sym.type + "* " + sym.reg + ", i32 0";
            for (int i = 0; i < dim; i++){
                output.push_back(", i32 " + index[i]);
                //cout << ", i32 " + index[i];
            }
            output.push_back("\n");
            //cout << endl;
            memory.push(sym.reg);
            reg = "%x" + to_string(new_reg);
        }        
        if (*token_iter == "="){
            token_iter++;
            Exp(symbols, false);
            output.push_back("    store i32 " + rtn.first + ", i32* " + reg + "\n");
            //cout << "    store i32 " + rtn.first + ", i32* " + reg << endl;
            if (*token_iter == ";"){
                token_iter++;
            }
            else{
                exit(1);
            }
        }
        else{
            exit(1);
        }
    }
    else if (*token_iter == "{"){
        Block(symbols);
    }
    else if (*token_iter == "if"){
        token_iter++;
        if (*token_iter == "("){
            token_iter++;
            int block_true, block_false, block_out;
            Cond(symbols, block_true, block_false);
            if (*token_iter == ")"){
                token_iter++;
                output.push_back("\nx" + to_string(block_true) + ":\n");
                //cout << "\nx" + to_string(block_true) + ":" << endl;
                Stmt(symbols);
                block_out = memory.size();
                memory.push("block_out");
                output.push_back("    br label %x" + to_string(block_out) + "\n");
                //cout << "    br label %x" + to_string(block_false) << endl;
                output.push_back("\nx" + to_string(block_false) + ":\n");
                //cout << "\nx" + to_string(block_false) + ":" << endl;
                if (*token_iter == "else"){
                    token_iter++;
                    Stmt(symbols);
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
    else if (*token_iter == "while"){
        token_iter++;
        if (*token_iter == "("){
            token_iter++;
            int block_cond, block_true, block_false;
            block_cond = memory.size();
            memory.push("block_cond");
            output.push_back("    br label %x" + to_string(block_cond) + "\n");
            //cout << "    br label %x" + to_string(block_cond) << endl;
            output.push_back("\nx" + to_string(block_cond) + ":\n");
            //cout << "\nx" + to_string(block_cond) + ":" << endl;
            Cond(symbols, block_true, block_false);
            if (*token_iter == ")"){
                token_iter++;
                int br_size = output.size(), cnt_size = output.size();
                output.push_back("\nx" + to_string(block_true) + ":\n");
                //cout << "\nx" + to_string(block_true) + ":" << endl;
                Stmt(symbols);
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
    else if (*token_iter == "break"){
        output.push_back("break_record");
        token_iter++;
        if (*token_iter == ";"){
            token_iter++;
        }
        else{
            exit(1);
        }
    }
    else if (*token_iter == "continue"){
        output.push_back("continue_record");
        token_iter++;
        if (*token_iter == ";"){
            token_iter++;
        }
        else{
            exit(1);
        }
    }
    else if (*token_iter == "return"){
        token_iter++;
        Exp(symbols, false);
        if (*token_iter == ";"){
            output.push_back("    ret i32 " + rtn.first + "\n");
            //cout << "    ret i32 " + rtn.first << endl;
            token_iter++;
        }
        else{
            exit(1);
        }
    }
    else if (*token_iter == ";"){
        token_iter++;
    }
    else{
        Exp(symbols, false);
        if (*token_iter == ";"){
            token_iter++;
        }
        else{
            exit(1);
        }
    }
}

void Exp(vector<symbol> &symbols, bool isConst){
    AddExp(symbols, isConst);
}

void Cond(vector<symbol> &symbols, int &block_exec, int &block_false){
    LOrExp(symbols, block_exec, block_false);
}

void LVal(vector<symbol> &symbols){
    if (!is_symbol(symbols, *token_iter) && get_sym_by_name(symbols, *token_iter).level < 0){
        exit(1);
    }
}

void PrimaryExp(vector<symbol> &symbols, bool isConst){
    if (*token_iter == "("){
        token_iter++;
        Exp(symbols, isConst);
        if (*token_iter == ")"){
            token_iter++;
        }
        else{
            exit(1);
        }
    }
    else if (isnum(*token_iter)){
        rtn.first = *token_iter;
        rtn.second = "i32";
        token_iter++;
    }
    else if (isIdent(*token_iter)){
        LVal(symbols);
        symbol sym = get_sym_by_name(symbols, *token_iter);
        if (isConst){
            if (sym.isConst && sym.type == "i32"){
                rtn.first = sym.reg;
                rtn.second = "i32";
                token_iter++;
            }
            else{
                exit(1);
            }
        }
        else{
            if (sym.isConst){
                if (sym.type == "i32"){
                    rtn.first = sym.reg;
                    rtn.second = "i32";
                    token_iter++;
                }
                else{
                    token_iter++;
                    int dim = 0;
                    string index[sym.dim];
                    while (*token_iter == "["){
                        token_iter++;
                        Exp(symbols, false);
                        index[dim] = rtn.first;
                        if (*token_iter == "]"){
                            dim++;
                            token_iter++;
                        }
                        else{
                            exit(1);
                        }
                    }
                    if (dim > sym.dim){
                        exit(1);
                    }
                    int new_reg = memory.size();
                    output.push_back("    %x" + to_string(new_reg) + " = getelementptr " + sym.type + ", " + sym.type + "* " + sym.reg + ", i32 0");
                    //cout << "    %x" + to_string(new_reg) + " = getelementptr " + sym.type + ", " + sym.type + "* " + sym.reg + ", i32 0";
                    for (int i = 0; i < dim; i++){
                        output.push_back(", i32 " + index[i]);
                        //cout << ", i32 " + index[i];
                    }
                    output.push_back("\n");
                    //cout << endl;
                    memory.push(sym.reg);
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
                if (sym.type == "i32"){
                    int new_reg = memory.size();
                    output.push_back("    %x" + to_string(new_reg) + " = load i32, i32* " + sym.reg + "\n");
                    //cout << "    %x" + to_string(new_reg) + " = load i32, i32* " + sym.reg << endl;
                    memory.push(sym.reg);
                    rtn.first = "%x" + to_string(new_reg);
                    rtn.second = "i32";
                    token_iter++;
                }
                else{
                    token_iter++;
                    int dim = 0;
                    string index[sym.dim];
                    while (*token_iter == "["){
                        token_iter++;
                        Exp(symbols, false);
                        index[dim] = rtn.first;
                        if (*token_iter == "]"){
                            dim++;
                            token_iter++;
                        }
                        else{
                            exit(1);
                        }
                    }
                    if (dim > sym.dim){
                        exit(1);
                    }
                    int new_reg = memory.size();
                    output.push_back("    %x" + to_string(new_reg) + " = getelementptr " + sym.type + ", " + sym.type + "* " + sym.reg + ", i32 0");
                    //cout << "    %x" + to_string(new_reg) + " = getelementptr " + sym.type + ", " + sym.type + "* " + sym.reg + ", i32 0";
                    for (int i = 0; i < dim; i++){
                        output.push_back(", i32 " + index[i]);
                        //cout << ", i32 " + index[i];
                    }
                    output.push_back("\n");
                    //cout << endl;
                    memory.push(sym.reg);
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

void UnaryExp(vector<symbol> &symbols, bool isConst){
    if (*token_iter == "(" || isnum(*token_iter) || isIdent(*token_iter)){
        if (isFunc(*token_iter)){
            function func = get_func_by_name(*token_iter);
            token_iter++;
            if (*token_iter == "("){
                token_iter++;
                string params;
                FuncRParams(symbols, func, params);
                if (*token_iter == ")"){
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
                    token_iter++;
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
            PrimaryExp(symbols, isConst);
        }
    }
    else if (*token_iter == "+" || *token_iter == "-" || *token_iter == "!"){
        bool minus = *token_iter == "-";
        bool inverse = *token_iter == "!";
        UnaryOp();
        UnaryExp(symbols, isConst);
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
    if (*token_iter == "+" || *token_iter == "-" || *token_iter == "!"){
        token_iter++;
    }
    else{
        exit(1);
    }
}

void FuncRParams(vector<symbol> &symbols, function func, string &params){
    int argc = 0;
    while (*token_iter != ")"){
        Exp(symbols, false);
        if (func.argv[argc] == rtn.second){
            params += (func.argv[argc] + " " + rtn.first);
            argc++;
        }
        else{
            exit(1);
        }
        if (*token_iter == ","){
            params += ", ";
            token_iter++;
        }
    }
    if (argc != func.argc){
        exit(1);
    }
}

void MulExp(vector<symbol> &symbols, bool isConst){
    UnaryExp(symbols, isConst);
    string var1, var2;
    while (*token_iter == "*" || *token_iter == "/" || *token_iter == "%"){
        var1 = rtn.first;
        string op = *token_iter;
        token_iter++;
        UnaryExp(symbols, isConst);
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

void AddExp(vector<symbol> &symbols, bool isConst){
    MulExp(symbols, isConst);
    string var1, var2;
    while (*token_iter == "+" || *token_iter == "-"){
        var1 = rtn.first;
        string op = *token_iter;
        token_iter++;
        MulExp(symbols, isConst);
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
        else{
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

void RelExp(vector<symbol> &symbols){
    AddExp(symbols, false);
    string var1, var2, var_temp = rtn.first;
    while (*token_iter == "<" || *token_iter == ">" || *token_iter == "<=" || *token_iter == ">="){
        var1 = var_temp;
        string op = *token_iter;
        token_iter++;
        AddExp(symbols, false);
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

void EqExp(vector<symbol> &symbols){
    RelExp(symbols);
    string var1, var2, var_temp = rtn.first;
    while (*token_iter == "==" || *token_iter == "!="){
        var1 = var_temp;
        string op = *token_iter;
        token_iter++;
        RelExp(symbols);
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

void LAndExp(vector<symbol> &symbols, int &or_block_next){
    EqExp(symbols);
    int block_next = memory.size();
    memory.push("block_next");
    or_block_next = memory.size();
    memory.push("upper_block_next_exec");
    output.push_back("    br i1 " + rtn.first + ", label %x" + to_string(block_next) + ", label %x" + to_string(or_block_next) + "\n");
    //cout << "    br i1 " + rtn.first + ", label %x" + to_string(block_next) + ", label %x" + to_string(or_block_next) << endl;
    while (*token_iter == "&&"){
        token_iter++;
        output.push_back("\nx" + to_string(block_next) + ":\n");
        //cout << "\nx" + to_string(block_next) + ":" << endl;
        EqExp(symbols);
        block_next = memory.size();
        memory.push("block_next");
        output.push_back("    br i1 " + rtn.first + ", label %x" + to_string(block_next) + ", label %x" + to_string(or_block_next) + "\n");
        //cout << "    br i1 " + rtn.first + ", label %x" + to_string(block_next) + ", label %x" + to_string(or_block_next) << endl;
    }
    output.push_back("\nx" + to_string(block_next) + ":\n");
    //cout << "\nx" + to_string(block_next) + ":" << endl;
}

void LOrExp(vector<symbol> &symbols, int &block_true, int &block_false){
    int block_next;
    LAndExp(symbols, block_next);
    int block_exec = memory.size();
    memory.push("block_exec");
    output.push_back("    br i1 " + rtn.first + ", label %x" + to_string(block_exec) + ", label %x" + to_string(block_next) + "\n");
    //cout << "    br i1 " + rtn.first + ", label %x" + to_string(block_exec) + ", label %x" + to_string(block_next) << endl;
    while (*token_iter == "||"){
        token_iter++;
        output.push_back("\nx" + to_string(block_next) + ":\n");
        //cout << "\nx" + to_string(block_next) + ":" << endl;
        LAndExp(symbols, block_next);
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
    if (isIdent(*token_iter)){
        token_iter++;
    }
    else{
        exit(1);
    }
}
