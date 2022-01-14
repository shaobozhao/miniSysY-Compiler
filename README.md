# Lab 4 实验报告

## 一、个人信息

- 姓名：赵少泊
- 学号：18377239

## 二、工作介绍

### Part 7 if 语句和条件表达式

&emsp;&emsp;Part 7内容的实现是在先前的各次实验的基础上完成的，同时对原先的代码进行了一定的优化与针对性的调整。本次实验主要的工作均是在语法分析程序`grammar.cpp`中完成的，除此之外还修改了词法分析器`lexer.cpp`中涉及到标识符`if`和`else`有关的部分。

&emsp;&emsp;根据实验文档中对语法规则的介绍，相应地新建了如下几个函数：

```cpp
void Cond(vector<element> &elements, int &block_exec, int &block_false);
void RelExp(vector<element> &elements);
void EqExp(vector<element> &elements);
void LAndExp(vector<element> &elements, int &or_block_next);
void LOrExp(vector<element> &elements, int &block_exec, int &block_false);
```

同时在`Stmt()`函数中就`if`-`else`条件语句的分析处理进行了较多的添加，下面结合代码进行说明：

```cpp
int block_true, block_false, block_out;
Cond(elements, block_true, block_false);
if (*sym == ")"){
    sym++;
    /* Begin block_true */
    Stmt(elements);
    /* Initialize block_out */
    /* Go to block_out */
    /* End block_true */
    /* Begin block_false */
    if (*sym == "else"){
        sym++;
        Stmt(elements);
    }
    /* End block_false */
    /* Begin block_out */
}
```

&emsp;&emsp;`block_true`、`block_false`和`block_out`分别表示条件为真、条件为假和出口三个基本块，其中`block_true`和`block_false`在`Cond()`中完成判断后初始化，而`block_out`则在`block_true`执行完毕后初始化。整个`if`-`else`条件语句大致的执行流程如上图代码所示。接下来叙述一下条件判断的执行流程。

&emsp;&emsp;在条件判断的各函数中，由于`RelExp()`和`EqExp()`与`AddExp()`较为相似，故不再做过多介绍。在`LAndExp()`和`LOrExp()`中提前采用了短路求值的方法进行了条件求值的实现。由于其文字叙述较为繁琐，故采用了伪代码的方式进行描述，其大致流程如下所示：

```cpp
void LAndExp(vector<element> &elements, int &or_block_next){
    EqExp(elements);
    /* Initialize block_next(next block_current) */
    /* Initialize or_block_next */
    /* If true, go to block_next; Else go to or_block_next */
    /* End block_current */
    while (*sym == "&&"){
        sym++;
        /* Begin block_current(previous block_next) */
        EqExp(elements);
        /* Initialize block_next(next block_current) */
        /* If true, go to block_next; Else go to or_block_next */
        /* End block_current */
    }
    /* Begin block_next(block_current in LOrExp())(rtn = 1) */
}

void LOrExp(vector<element> &elements, int &block_true, int &block_false){
    LAndExp(elements, block_next);
    /* Initialize block_exec */
    /* If true, go to block_exec; Else go to block_next(or_block_next in LAndExp()) */
    /* End block_current */
    while (*sym == "||"){
        sym++;
        /* Begin block_current(previous block_next) */
        LAndExp(elements, block_next);
        /* If true, go to block_exec; Else go to block_next */
        /* End block_current */
    }
    /* Begin block_exec */
    /* Initialize block_true */
    /* Go to block_true */
    /* End block_exec */

    /* Begin block_next(last or_block, rtn = 0) */
    /* Initialize block_false */
    /* Go to block_false */
    /* End block_next */
}
```

&emsp;&emsp;由于此时尚未使用代码回填的技术，且跳转block的值是提前分配好的，故寄存器无法再使用简单的数字递增，其格式由“%n”修改为“%xn”。

## 三、思考题

&emsp;&emsp;本次实验无思考题。

## 四、复用与借鉴

&emsp;&emsp;在完成本次实验的过程中，复用或借鉴参考代码或其他实现的情况并没有出现。
