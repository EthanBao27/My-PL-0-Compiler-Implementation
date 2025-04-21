#ifndef PL0_PARSER_H
#define PL0_PARSER_H

#include <string>
#include <vector>
#include <unordered_map>

/* 传进来的 token = (type, lexeme) */
struct RawToken {
    std::string type;   /* 如 "constsym" / "plus" / "ident" ... */
    std::string lexeme; /* 对应词素               */
};

/* 枚举化后方便 switch */
enum class Tok {
    /* 关键字 */
    BEGINSYM, ENDSYM,
    CONSTSYM, VARSYM, PROCEDURESYM, CALLSYM,
    IFSYM, ELSESYM, THENSYM, WHILESYM, DOSYM, ODDSYM,
    READSYM, WRITESYM,
    /* 标识符 / 常数 */
    IDENT, NUMBER,
    /* 运算符与界符 */
    PLUS, MINUS, TIMES, SLASH,
    EQL, NEQ, LSS, LEQ, GTR, GEQ,
    BECOMES,
    LPAREN, RPAREN, COMMA, SEMICOLON, PERIOD,
    END       // 虚拟 EOF
};

// 符号表项结构，用于语义检查
struct Symbol {
    enum class Type { CONST, VAR, PROCEDURE };
    std::string name;  // 符号名称
    Type type;         // 符号类型
    int value;         // 常量值（如果是常量）
    int level;         // 嵌套层级

    Symbol(const std::string& n, Type t, int l, int v = 0)
        : name(n), type(t), level(l), value(v) {}
};

class Parser {
public:
    explicit Parser(const std::vector<RawToken>& raw); /* 构造时完成映射 */
    void parse();                                      /* 主入口         */
    int getErrorCount() const { return errorCount; }   /* 获取错误计数   */
private:
    /* 内部实现隐藏 */
    struct Token { Tok t; std::string lex; };
    std::vector<Token> toks;
    size_t pos = 0;
    bool errorRecoveryMode = false;  // 错误恢复模式标记
    int errorCount = 0;              // 错误计数器

    // 符号表及相关
    std::vector<Symbol> symbolTable;
    int currentLevel = 0;

    /* 小工具 */
    Token& cur();     bool is(Tok);  void adv();
    void err(const std::string&);
    void reportError(const std::string& message);  // 报告错误但不退出
    void errorRecovery(const std::vector<Tok>& syncTokens);  // 错误恢复
    
    // 符号表操作
    void enterSymbol(const std::string& name, Symbol::Type type, int value = 0);
    bool symbolExists(const std::string& name, int level = -1);
    Symbol* findSymbol(const std::string& name);

    /* 文法函数 */
    void program();  void block();
    void constDecl(); void varDecl();
    void statement(); void condition();
    void expression(); void term(); void factor();

    void expect(Tok t);
};

static std::string tokToString(Tok t)
{
    switch (t) {
        case Tok::BEGINSYM: return "begin";
        case Tok::ENDSYM: return "end";
        case Tok::CONSTSYM: return "const";
        case Tok::VARSYM: return "var";
        case Tok::PROCEDURESYM: return "procedure";
        case Tok::CALLSYM: return "call";
        case Tok::IFSYM: return "if";
        case Tok::ELSESYM: return "else";
        case Tok::THENSYM: return "then";
        case Tok::WHILESYM: return "while";
        case Tok::DOSYM: return "do";
        case Tok::ODDSYM: return "odd";
        case Tok::READSYM: return "read";
        case Tok::WRITESYM: return "write";
        case Tok::IDENT: return "标识符";
        case Tok::NUMBER: return "常数";
        case Tok::PLUS: return "+";
        case Tok::MINUS: return "-";
        case Tok::TIMES: return "*";
        case Tok::SLASH: return "/";
        case Tok::EQL: return "=";
        case Tok::NEQ: return "<>";
        case Tok::LSS: return "<";
        case Tok::LEQ: return "<=";
        case Tok::GTR: return ">";
        case Tok::GEQ: return ">=";
        case Tok::BECOMES: return ":=";
        case Tok::LPAREN: return "(";
        case Tok::RPAREN: return ")";
        case Tok::COMMA: return ",";
        case Tok::SEMICOLON: return ";";
        case Tok::PERIOD: return ".";
        case Tok::END: return "<EOF>";
        default: return "<未知符号>";
    }
}

#endif