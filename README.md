# Lab 1 实验报告

## 一、个人信息

- 姓名：赵少泊
- 学号：18377239

## 二、工作介绍

### Part 1 仅有 main 函数与 return 的编译器

&emsp;&emsp;Part 1部分的完成基本上是在先前的词法分析实验的代码的基础上实现的，但同时做出了一些改动，如将分割后的字符串token的词法分析处理函数修改为`process()`，标识符的分析函数修改为`process_word()`，以及数字的分析处理函数修改为`process_number()`。在修改处理函数的过程中考虑了针对接下来实验向下兼容的情况，提前实现了可能用到的功能（如对`if`、`else`等标识符的处理），为避免对本次实验结果的影响将其注释。

&emsp;&emsp;在进行语法分析的过程时采用了递归下降的方法手动构建了语法树。根据课上所讲的方法和实验要求，构建了`Stmt()`、`Block()`等函数，并以`CompUnit()`为语法分析的入口，在主函数中对其进行调用。在之前的处理过程中，将词法分析的输出以字符串的形式插入到容器`syms`中，将生成代码字符串插入到容器`items`中，在完成语法分析后将`items`的内容输出到文件：

```cpp
sym = syms.begin();
CompUnit();
if (sym == syms.end() && !multi_line_note){
    for (int i = 0; i < items.size(); i++){
        ir << items[i];
    }
}
```

在词法分析和语法分析的过程中，一旦发现错误，即调用`exit()`以值1退出。

### Part 2 消除注释

&emsp;&emsp;设置全局布尔变量`single_line_note`和`multi_line_note`对当前是否处于注释区域内进行标记。为了识别注释开始和多行注释结束的位置，分别构造了两个函数`check_note_start()`和`check_multi_note_end()`，检查当前token中是否包含注释的开始标志`//`或`/*`和多行注释的结束标志`*/`，若有则对token中标志以前（以后）的部分进行词法和语法分析及处理，并对`single_line_note`和`multi_line_note`作出相应修改。在主函数中，进行逐行识别文本时调用上述的识别函数：

```cpp
while (line_split >token){
    if (!single_line_note && !multi_line_note){
        check_note_start(token);
        check_multi_note_end(token);
    }
    else if (!single_line_note && multi_line_note){
        check_multi_note_end(token);
    }
    else;
}
line_split.str("");
items.push_back("\n");
single_line_note = false;
```

此外，在扫描到达文本结尾处，即将输出生成代码时对是否仍处于多行代码中进行判断，若是则以1退出。

## 三、思考题

&emsp;&emsp;通过仅对注释标识符前/后的语句进行分析处理来消除注释，具体的实现流程可参见工作流程中的Part 2部分。

## 四、复用与借鉴

&emsp;&emsp;在完成本次实验的过程中，复用或借鉴参考代码或其他实现的情况并没有出现。
