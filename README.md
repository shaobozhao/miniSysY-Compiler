# Lab 8 实验报告

## 一、个人信息

- 姓名：赵少泊
- 学号：18377239

## 二、工作介绍

### Part 13 函数

&emsp;&emsp;Part 13内容的实现是在先前的各次实验的基础上完成的，但对原先的代码进行了较大规模的优化与针对性的调整，对多个变量和函数的命名进行了规范的统一，同时再次对程序的整体结构进行了调整，将部分共通性的内容提到`public.h`中，同时将`grammar.cpp`修改为`parser.cpp`，以做到名副其实。本次实验主要的工作均是在`parser.cpp`中完成的。

&emsp;&emsp;在本次实验中，对语法树的构造函数进行了新的调整，新加入了表示函数参数和参数列表的函数：

```cpp
void FuncFParams(stack<string> &memory, vector<symbol> &symbols, function &func, vector<symbol> &params);
void FuncFParam(stack<string> &memory, vector<symbol> &symbols, function &func, vector<symbol> &params);
```

除此之外，将表示栈的全局变量`memory`更改为在`FuncDef()`中初始化的局部变量，并通过参数在不同层级函数间进行传递。

&emsp;&emsp;在整个实验中，重难点之一就是对函数参数的处理。当形式参数在`FuncFParam()`完成初步的定义之后，还需要在函数中完成相应的分配空间并进行相应的储存，这一过程在`FuncDef()`中完成：

```cpp
for (int i = 0; i < func.argc; i++){
    int pos = symbols.size() - func.argc + i; // symbols[pos]，在符号表中找到对应参数
    int new_reg = memory.size();
    output.push_back("    %x" + to_string(new_reg) + " = alloca " + symbols[pos].type + "\n"); // 为参数分配空间
    output.push_back("    store " + symbols[pos].type + " " + symbols[pos].reg + ", " + symbols[pos].type + "* %x" + to_string(new_reg) + "\n"); // 完成储存
    memory.push(symbols[pos].reg);
    symbols[pos].reg = "%x" + to_string(new_reg); // 修改参数对应的寄存器
}
```

&emsp;&emsp;实验中的另一个难点（自认为的）是数组指针的处理。其实现的过程算不上非常复杂，只需要在Lab 7的基础上稍加改动即可。但个人认为这一块并不容易想到，而且在利用llvm进行检测的过程中多次出现类型不匹配的bug，在这一块上消耗了较多的时间和经历。

&emsp;&emsp;实验中还需要思考的问题是使所有分支都带有`return`语句，即让函数的“根`Block`”带有`return`语句。解决的方法使增加全局布尔变量`no_return`对“根`Block`”（`level` = 1）的`return`的含有情况进行检查，若在“根`Block`”退出时`no_return`的值仍为`true`，则手动地增加`return`语句：

```cpp
if (no_return){
    if (!void_func){
        output.push_back("    ret i32 0\n");
    }
    else{
        output.push_back("    ret void\n");
    }
}
```

## 三、思考题

&emsp;&emsp;本次实验无思考题。

## 四、复用与借鉴

&emsp;&emsp;在完成本次实验的过程中，复用或借鉴参考代码或其他实现的情况并没有出现。
