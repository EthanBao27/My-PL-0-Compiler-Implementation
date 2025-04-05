# pl/0 lexier 实现

使用了 DFA 确定性有限状态自动机算法，定义状态列表 state，通过读取字符流，达到状态转移。

---

使用方法: `lexier_main.cpp`中设定读取 txt 文件路径，
运行：

```bash
g++ -std=c++11  lexer_main.cpp -o lexer
```

编译链接生成目标文件，
运行:

```bash
./lexer
```

查看运行结果。
