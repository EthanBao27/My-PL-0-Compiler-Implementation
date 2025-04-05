#include <iostream>
#include <vector>
#include <map>
#include <set>

using namespace std;

#define NRW 11 // number of reserved words 保留词数量
#define MAXNUMLEN 14 // 数字最大位
#define MAXIDLEN 10 // 标识符最大长度

/**
 * @brief 定义DFA有限状态机的状态
 * 
 */
enum state
{
    // 初始状态
    START,
    // 读取数字序列
    INNUM,
    // 读取标识符或关键字（字母开始）
    INID,
    // 读取赋值操作符 :=
    INBECOMES,
    // 识别出完整的赋值操作符
    BECOMES,
    // 检测到>符号
    GTR,
    // 检测到>=符号
    GEQ,
    // 检测到!=符号
    NEQ,
    // 检测到<符号
    LES,
    // 检测到<=符号
    LEQ,
    // 终止状态
    END,
    // 读取注释
    COMMENT
};

// 初始状态
state currentState = START;

// 定义Pl/0语言词汇表
// 基本字 单词-符号(symbol)
map<string, string> keywords = {
    {"begin", "beginsym"},
    {"call", "callsym"},
    {"const", "constsym"},
    {"do", "dosym"},
    {"end", "endsym"},
    {"if", "ifsym"},
    {"odd", "oddsym"},
    {"procedure", "proceduresym"},
    {"read", "readsym"},
    {"var", "varsym"},
    {"while", "whilesym"},
    {"write", "writesym"}
};

// 标识符
const string identifier="ident";

// 常数
const string number = "number";

// 运算符
map<string, string> operators = {
    {"+", "plus"},
    {"-", "minus"},
    {"*", "times"},
    {"/", "slash"},
    {"=", "eql"},
    {"<>", "neq"},
    {"#", "neq"},
    {"<", "lss"},
    {"<=", "leq"},
    {">", "gtr"},
    {">=", "geq"},
    {":=", "becomes"}
};

// 界符号（分隔符）
map <string,string> delimiters={
    {"(","lparen"},
    {")","rparen"},
    {",","comma"},
    {";","semicolon"},
    {".","period"}
};

// 报错提示
const char* err_msg[] =
{
/*  0 */    "Fatal Error:Unknown character.\n",
/*  1 */    "Found ':=' when expecting '='.",
/*  2 */    "There must be a number to follow '='.",
/*  3 */    "There must be an '=' to follow the identifier.",
/*  4 */    "There must be an identifier to follow 'const', 'var', or 'procedure'.",
/*  5 */    "Missing ',' or ';'.",
/*  6 */    "Incorrect procedure name.",
/*  7 */    "Statement expected.",
/*  8 */    "Follow the statement is an incorrect symbol.",
/*  9 */    "'.' expected.",
/* 10 */    "';' expected.",
/* 11 */    "Undeclared identifier.",
/* 12 */    "Illegal assignment.",
/* 13 */    "':=' expected.",
/* 14 */    "There must be an identifier to follow the 'call'.",
/* 15 */    "A constant or variable can not be called.",
/* 16 */    "'then' expected.",
/* 17 */    "';' or 'end' expected.",
/* 18 */    "'do' expected.",
/* 19 */    "Incorrect symbol.",
/* 20 */    "Relative operators expected.",
/* 21 */    "Procedure identifier can not be in an expression.",
/* 22 */    "Missing ')'.",
/* 23 */    "The symbol can not be followed by a factor.",
/* 24 */    "The symbol can not be as the beginning of an expression.",
/* 25 */    "The number is too great.",
/* 26 */    "The identifier is too long",
/* 27 */    "",
/* 28 */    "",
/* 29 */    "",
/* 30 */    "",
/* 31 */    "",
/* 32 */    "There are too many levels."
};

// 实用函数 (utils)
// 判断是否是关键字
bool isKeyword(const string &str){
    return keywords.find(str) != keywords.end();
}

// 判断是否是运算符
bool isOperator(const char *c){
    return operators.find(c) != operators.end();
}
// 判断是否是单字符运算符
bool isSingleOperator(const char c){
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '#');
}

// 判断是否是数字
bool isNumber(const string &str){
    for(char c:str){
        if(!isdigit(c)){
            return false;
        }
    }
    return true;
}

// 判断是否是分隔符
bool isDelimiter(const string &str){
    return delimiters.find(str) != delimiters.end();
}

// 清理当前识别token内存
void cleanTokenMem(char* cur_token,int &cur_token_index){
    memset(cur_token, 0, MAXIDLEN+1);
    cur_token_index = 0;
}