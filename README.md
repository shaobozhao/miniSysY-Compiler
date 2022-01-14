# Lab 6 实验报告

## 一、个人信息

- 姓名：赵少泊
- 学号：18377239

## 二、工作介绍

### Part 10 循环语句

&emsp;&emsp;Part 10内容的实现是在先前的各次实验的基础上完成的，同时对原先的代码进行了一定的优化与针对性的调整。本次实验主要的工作均是在语法分析程序`grammar.cpp`中完成的，此外还需要再`lexer.cpp`中增加对`while`的词法分析和处理。

&emsp;&emsp;和Lab 4中`if`-`else`条件语句的实现原理相似，`while`循环同样是依赖于条件判断语句`Cond()`进行并根据判断结果在基本块间进行跳转，故其实现过程也与之大致相似（不同点在于`while`中出口`block_out`和条件为假`block_false`的两个块是一致的），其大致流程的伪代码表示如下（包括了部分Part 11中的内容）：

```cpp
int block_cond, block_true, block_false;
/* Initialize block_cond */
/* Go to block_cond */
/* Begin block_cond */
Cond(elements, block_true, block_false);
/* End block_cond */
if (*sym == ")"){
    sym++;
    int br_size = output.size(), cnt_size = output.size();
    /* Begin block_true */
    Stmt(elements);
    for (int i = br_size; i < output.size(); i++){
        if(output[i] == "continue_record"){
            /* Update output[i] */
            /* Go to block_cond */
        }
    }
    for (int i = cnt_size; i < output.size(); i++){
        if(output[i] == "break_record"){
            /* Update output[i] */
            /* Go to block_false */
        }
    }
    /* Go to block_cond */
    /* End block_true */
    /* Begin block_false */
}
```

### Part 11 continue、break 与代码回填

&emsp;&emsp;和`while`一样，对`break`和`contunue`相关的处理同样在`Stmt()`中进行。由于当识别到`break`和`continue`标识符时，其应跳转的`Block`尚未完成初始化，故先在相应位置以对应的`record`填充位置（如下所示）；当相应的`Block`完成初始化后，将`record`更新为`Block`的跳转指令（如Part 10中伪代码所示），完成代码回填。

```cpp
else if (*sym == "break"){
    output.push_back("break_record");
    /* ...... */
}
    
else if (*sym == "continue"){
    output.push_back("continue_record");
    /* ...... */
}
```

## 三、思考题

&emsp;&emsp;本次实验无思考题。

## 四、复用与借鉴

&emsp;&emsp;在完成本次实验的过程中，复用或借鉴参考代码或其他实现的情况并没有出现。
