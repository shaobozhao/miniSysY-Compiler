# Lab 3 实验报告

## 一、个人信息

- 姓名：赵少泊
- 学号：18377239

## 二、工作介绍

### Part 5 局部变量与赋值

&emsp;&emsp;Part 5内容的实现是在先前的各次实验的基础上完成的，同时对原先的代码进行了一定的优化与针对性的调整。本次实验主要的工作均是在语法分析程序`grammar.cpp`中完成的。

&emsp;&emsp;根据实验文档的介绍，由于本次实验开始对变量或常量名有了要求，故根据理论课上学到的内容，新建了符号结构体`element`：

```cpp
struct element{
    string name;
    bool isConst;
    string type;
    string reg;
};
```

同时利用`vector`容器构建了相应的符号表`elements`（考虑到后续关于作用域和全局变量的实现，`elements`选择了以局部变量的方式进行参数传递在不同层次间调用的方式实现），并构建了相应的判断函数`is_element()`和调用函数`get_elem_by_name()`。

&emsp;&emsp;根据实验文档中给出的语法规则，对语法树构建及分析处理的相关函数进行了适当的添加和修改（为部分先前已存在的函数增加了参数）：

```cpp
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
```

&emsp;&emsp;总体上语法和语义分析及处理的部分并没有太多特别值得注意的重难点，主要关注的问题是常量和常量表达式。此时的常量并不如同变量一般调用`alloca`分配空间和寄存器，而是直接保存相应的值；类似地，常量表达式也不通过占用栈和寄存器进行运算，返回值`rtn`储存的是数值计算的结果值。在表达式计算时，通过布尔类型参数`isConst`对是否为常量表达式进行判断，以决定计算时使用的方法（数值计算/运算指令）。

### Part 6 调用函数

&emsp;&emsp;根据实验文档的介绍，由于需要支持函数的调用，故新建了函数结构体`function`，并和`element`一样增加了对应的判断和调用函数：

```cpp
struct function{
    string name;
    string type;
    int argc;
    vector<string> argv;
};
```

考虑到之后有自定义函数的需求，故将库函数列表`miniSysY`和已调用函数列表`functions`（将来包含自定义的函数）分开处理，在第一次调用某库函数时将其加入`functions`：

```cpp
const function getint = {.name = "getint", .type = "int", .argc = 0, .argv = {}},
                        /* ...... */
const vector<function> miniSysY = {getint, getch, getarray, putint, putch, putarray};

vector<function> functions;
```

&emsp;&emsp;在调用函数时，还需要对函数的调用传入的实参列表与函数的形参列表长度和各参数类型进行比较，为此新建了函数`FuncRParams`，在其中通过遍历对参数列表的合法性进行判断，若出现不一致则立即以1退出。

```cpp
void FuncRParams(vector<element> &elements, function func, string &params, bool isConst);
```

## 三、思考题

&emsp;&emsp;本次实验无思考题。

## 四、复用与借鉴

&emsp;&emsp;在完成本次实验的过程中，复用或借鉴参考代码或其他实现的情况并没有出现。
