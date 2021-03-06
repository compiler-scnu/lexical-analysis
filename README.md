# lexical-analysis
## 实验内容：

1. C++源代码扫描程序识别C++记号。
   - C++语言包含了几种类型的记号：标识符，关键字，数（包括整数、浮点数），字符串、注释、特殊符号（分界符）和运算符号等。
2. 打开一个C++源文件，打印出所有以上的记号。
3. 要求应用程序应为Windows界面。
4. 选作部分：为了提高C++源程序的可读性，C++程序在书写过程中加入了空行、空格、缩进、注释等。
   - 假设你想牺牲可读性，以节省磁盘空间，那么你可以存贮一个删除了所有不必要空格和注释的C++源程序的压缩文本。
   - 因此，程序中还应该有这样的压缩功能。
5. 选作部分：进一步思考或实现——如何进一步实现减小源文件大小的压缩功能。
6. 应该书写完善的软件文档。

## 运行效果样本

C++源程序：

Test.cpp

```c++
#include<iostream.h>
main()
{
   int i;
   cin>>i;
   if (i>3) cout<<“ok”;
}
```




扫描结果：

> \#	特殊符号
>
> Include    关键字
>
> <      特殊符号
>
> iostream. h    关键字
>
> \>      特殊符号
>
> main    关键字
>
> (        特殊符号
>
> )        特殊符号
>
> {      特殊符号
>
> int    关键字
>
> i      标识符
>
> ;       特殊符号
>
> cin   关键字
>
> \>>    特殊符号
>
> i       标识符
>
> ;       特殊符号
>
> if      关键字       
>
> (       特殊符号
>
> i       标识符
>
> \>      特殊符号
>
> 3       数
>
> )        特殊符号
>
> cout   关键字
>
> <<     特殊符号
>
> “ok”   串
>
> ;        特殊符号
>
> }        特殊符号

