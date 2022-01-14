# Lab 2 实验报告

## 一、个人信息

- 姓名：赵少泊
- 学号：18377239

## 二、工作介绍

### Part 3 实现正号、负号

&emsp;&emsp;Part 3内容的实现是在先前的各次实验的基础上完成的，同时对原先的代码进行了一定的优化与针对性的调整。由于此前的程序文件`compiler.cpp`在完成本部分后，若仍保持所有代码集中于同一文件的状态则会导致代码段的过于冗长以及高度的混乱复杂，故根据功能的不同作出如下分割：将与词法分析相关的变量和函数转移至新建的`lexer.cpp`中；将与语法、语义分析和代码生成的变量和函数转移至新建的`grammar.cpp`中；在`compiler.cpp`中仅保留主函数`main()`和注释检查的相关函数。

&emsp;&emsp;根据实验文档的介绍，对语法树构建及分析处理的相关函数进行了适当的添加：

```cpp
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
```

&emsp;&emsp;本部分中，最为关键的内容即是对表达式取相反数的处理，故主要讲述这一部分的内容。根据文档中的说明与提示，在计算表达式时，采用了一种对内存栈和寄存器进行模拟的方式进行处理。由于采用数字寄存器时，寄存器的标号是递增的，故将栈的大小赋给寄存器的标号以保证这一规定。由于`UnaryOp`为正号`+`时表达式的值实际上并没有发生变化，故不对其进行处理；在`UnaryOp`为负号`-`时以0为被减数，当前表达式为减数进行减法运算，得到的差即为表达式的负。以全局变量`rtn`作为返回值保存当前运算结果对应的“寄存器”标号。下述即为具体的代码实现：

```cpp
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
```

### Part 4 实现四则运算及模运算

&emsp;&emsp;本部分的实现具体方式与Part 3中表达式取反的方法差异不大，且`AddExp()`与`MulExp()`的实现方法也基本一致，故在此仅以乘、除和模运算`MulExp()`为例进行简要说明。由于`*`、`/`和`%`均为双目运算符，且需要调用先前的表达式结果，故新建变量`var1`和`var2`用于存储上一级表达式的结果，并参与运算，其余部分和取反运算大同小异。

```cpp
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
```

## 三、思考题

&emsp;&emsp;本次实验无思考题。

## 四、复用与借鉴

&emsp;&emsp;在完成本次实验的过程中，复用或借鉴参考代码或其他实现的情况并没有出现。
