# pl/0 编译器-语法分析实现

---

Implementation of parser.
使用递归下降分析法实现语法分析

编译链接文件

```bash
g++ -std=c++11 main.cpp parser.cpp -o parser
```

运行

```bash
./parser ./tests/input.txt
```

## 可视化 AST 树

macOS 安装 Graphviz

```bash
brew install graphviz
```

生成 AST 树

```bash
./parser ./tests/case01.txt > ./out/ast/tree01.txt
python tree2dot.py ./out/ast/tree01.txt ./out/dot/tree01.dot
dot -Tpng ./out/dot/tree01.dot -o ./out/img/tree01.png
```

注意要把最后一行“语法正确”中文删掉.
